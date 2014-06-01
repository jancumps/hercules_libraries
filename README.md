hercules_libraries
==================
Set of libraries for the Texas Instruments  Hercules microcontroller family.

License
-------
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

i2cbb
=====
I2C master bit bang library. Software implementation for the protocol for these cases where the processor
doesn't have the peripherals or when you're out of peripherals.

lcd
===
YouTube videos:
---------------

TI Hercules LaunchPad with Arduino LCD shield (SainSmart LCD Keypad) part 1
http://youtu.be/gaA7ILWg-4E

TI Hercules LaunchPad with Arduino LCD shield (SainSmart LCD Keypad) part 2
http://www.youtube.com/watch?v=du8NFcbHSuM

This code is not my invention. I took the LiquidCrystal library from arduino and ported from C++ to C, and from arduino to TI hercules launchpad


Software:
---------
How to use it:
I expect you already know how to work with HALCoGen and Code Composer Studio from following Project0 on the Hercules LaunchPad wiki.

1:
First, start HALCoGen and create a new project. Select your correct processor, and name the project LCD

2:
go to the "Driver Enable" tab, and only select "GIO" and "HET".
File -> Save Project
File -> Generate Code
Close HALCoGen

3:
start CCSvX
File -> -> New -> CCS project
Project Name: LCD
In Device, select your processor.
In Project Templates, select "Empty Project"
-> press Finish

4: In the project explorer,
right click on project "LCD" -> Properties
select Build -> ARM Compiler -> Include Options
In the box "Add dir to #include search path", click + button.
Click Workspace
Select LCD - include
Press OK
Press OK
Press OK

5: download the 3 source files from githup.
place sys_main.c and 44780.c in the source directory of your CCSv LCD project.
place 44780.h in the include directory.

6: compile.

The Launchpad code is now ready.


Hardware:
---------

header layout of launchpad:
http://processors.wiki.ti.com/images/0/02/Hercules_LaunchPad_Header.pdf﻿

LCD Shield schema:
http://www.sainsmart.com/sainsmart-1602-lcd-keypad-shield-for-arduino-duemilanove-uno-mega2560-mega1280.html

attention: if you follow this schematic, never drive D10 of the shield high. It can only have 0 or open.
Driving it high would bias the basis-emittor junction of Q1 and you have a shortcut from your pin to ground.
I haven't tried this, but expect that if you want to PWM the brightness, you'll have to define the output pin as open collector.


You can choose what pins to use, but if you use my system_main.c, you have to connect following pins:

LCD DB4 -> LP J4:2 (NHET[2])
LCD DB5 -> LP J4:3 (NHET[4])
LCD DB6 -> LP J4:4 (NHET[6])
LCD DB7 -> LP J4:5 (NHET[10])

LCD RS -> LP J4:6 (NHET[12])
LCD ENABLE -> LP J4:7 (NHET[14])

LCD VSS -> LP J3:2 (GND)
LCD ~RW -> LP J3:2 (GND)
LCD VDD -> LP J3:1 (5V)



