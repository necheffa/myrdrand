Abstract:
  The rdrand instruction set is available in most newer Intel processors and
  provides a high bandwidth pseudo random number generator.

  But accessing it can be difficult.

  myrdrand provides a character device interface allowing the use of traditional
  Unix utilities to access the rdrand instruction set.

Prereqs:
  You must have an Intel Ivy Bridge processor or newer, with support for the
  rdrand instruction set.
  If you are unsure, try `grep -i rdrand /proc/cpuinfo` which should show
  a match if the rdrand instruction set is available.

  This code was developed and tested on Gentoo Linux using an unpatched
  4.4.19 kernel. But any distro using a 2.6 kernel or newer ought to work
  just fine.

Installing:
  You should have a configured Linux source directory first. Copy the
  myrdrand source directory into the root of the Linux source directory.
  Then cd into the myrdrand directory and execute make.

  With the module object built, execute the supplied run script to load
  the compiled module and create a device node under /dev.

Usage:
  The supplied run script will create a /dev/myrdrand character device node.
  This virtual file can in turn be used with tools like `cat` or `dd` - any
  of the ways you might access /dev/zero you could access /dev/myrdrand.

  The typical method to get random data on Linux is through /dev/random or
  /dev/urandom. And for applications where security is a priority, you should
  continue using these methods - myrdrand is not a suitable substitute. The
  urandom and random character devices use the rdrand instruction set, if
  available, in combination with other sources to create a "mixed pot" of
  randomness. This "mixed pot" ensures that any single bad actor would need
  to weaken all of the sources mixed into the pot to have a chance at predicting
  the random values and use this predictability to undermine your security.

  However, this "mixed pot" method is as slow as the slowest source mixed into
  the random pot. So for certain applications where security is not a priority,
  this may be undesireable. And it can even strain the available quality randomness
  for applications that do need cryptographic quality randomness.

  Intel claims that rdrand is suitable for cryptographic applications - but
  you really shouldn't rely on a single source for randomness the way myrdrand
  does.

