#!/bin/sh

insmod ./myrdrand.ko
major=$(grep myrdrand /proc/devices | awk '{print $1}')
mknod /dev/myrdrand c $major 0
