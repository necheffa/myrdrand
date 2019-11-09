/*
myrdrand provides a character device driver for full-bandwidth access to
the rdrand instruction set found on modern Intel CPUs.
Copyright (C) 2016, 2019  Alexander Necheff

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; using version 2
of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

/*  ChangeLog:
 *    2016-08-21 - Alexander Necheff - initial version.
 *    2019-11-09 - Alexander Necheff - added version number, added GPLv2 license
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/types.h>
#include <linux/slab.h>
#include <linux/cdev.h>
#include <linux/kernel.h>
#include <linux/errno.h>
#include <asm/uaccess.h>

static const int COUNT = 1;
static const int BUF_LEN = 1024;
static const char *VERSION = "v0.3";

static int myrdrand_major;
static struct cdev *myrdrand_cdev;

MODULE_LICENSE("GPLv2");
MODULE_AUTHOR("Alexander Necheff");

static int rdrand64_step(u64 *rand)
{
    unsigned char ok;
    asm volatile ("rdrand %0; setc %1"
            : "=r" (*rand), "=qm" (ok));

    return (int) ok;
}

int myrdrand_open(struct inode *inode, struct file *filp)
{
    return 0;
}

int myrdrand_release(struct inode *inode, struct file *filp)
{
    return 0;
}

ssize_t myrdrand_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos)
{

    void *rand_buf;
    unsigned long err = 0;
    u64 i = 0;
    u64 size = 0;

    if (count % 8) {
        size = count / 8 + 1;
    } else {
        size = count / 8;
    }

    rand_buf = kmalloc(sizeof(u64) * size, GFP_KERNEL);
    for (i = 0; i < size; i++) {
        int res = rdrand64_step((u64 *)rand_buf + i);
    }

    err = copy_to_user(buf, rand_buf, count);
    if (err) {
        printk(KERN_WARNING "myrdrand_read: %lu bytes not copied to caller. size is [%llu]. count is [%zu]\n", err, size, count);
        return -EFAULT;
    }

    kfree(rand_buf);
    return count; // provided nothing else fails, rdrand should always give us values to satisfy count
}

struct file_operations myrdrand_fops = {
    .owner = THIS_MODULE,
    .read = myrdrand_read,
    .open = myrdrand_open,
    .release = myrdrand_release,
};


static int myrdrand_init(void)
{
    dev_t dev;
    int res = -ENODEV;
    int err = 0;

    printk(KERN_ALERT "myrdrand: version %s is starting up.\n", VERSION);

    res = alloc_chrdev_region(&dev, 0, COUNT, "myrdrand");
    myrdrand_major = MAJOR(dev);
    if (res < 0) {
        printk(KERN_WARNING "myrdrand: can't get major.\n");
        return res;
    }

    myrdrand_cdev = cdev_alloc();
    myrdrand_cdev->ops = &myrdrand_fops;
    myrdrand_cdev->owner = THIS_MODULE;
    err = cdev_add(myrdrand_cdev, dev, COUNT);
    if (err) {
        printk(KERN_NOTICE "myrdrand: error adding cdev: %d\n", err);
    }

    return 0;
}

static void myrdrand_exit(void)
{
    printk(KERN_ALERT "myrdrand: version %s is shutting down.\n", VERSION);

    unregister_chrdev_region(MKDEV(myrdrand_major, 0), COUNT);
    cdev_del(myrdrand_cdev);
}

module_init(myrdrand_init);
module_exit(myrdrand_exit);

