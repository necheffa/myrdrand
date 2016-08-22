You _MUST_ have an Intel Ivy Bridge processor or newer with support for rdrand.

To install you should have a configured Linux source directory first.
Copy the myrdrand directory into the root of the Linux source directory.
cd into myrdrand and execute make
Run the run.sh script to insert the compiled module and create a device node.
???
Profit.


This source code has been compiled against and tested using Linux 4.4.19.
This source code is mostly awful and should not be used for much of anything;
I wrote it casually as an exploration.
