hercules_libraries
==================
Set of libraries for the Texas Instruments  Hercules microcontroller family.

License
=======
Free for all.
It's the intention that all the code submitted here can be used without any restriction.
If you use, alter or extend this code, please do that in a way that introduces no restrictions for others to use, alter or extend it.
If code or ideas from other locations are used in this repository, then that is documented in the source header, including a
reference of the license used by that code.


GioUtils
========
Utility functions to treat ports and pins as a single entitiy when performing gio activities.
These functions use a gioPin_t struct instead of separate port and pin parameters. 
It makes it easier to store info needed to drive or read a pin in a single variable.

