FitPet v0.5: Back to School Edition
======

Description
===========

Senior Design Project. Wearable pet that depends on your health to keep it healthy.

TODO:
=====
###Overall:
- Decide on what exactly the pets will be. (Pokemon sprites? Original, customizable sprites? Animated?)

###Software:
- Integrate Accelerometer, RTC
- Write menus and create UI (Eddy)
- Decide on memory locations on EEPROM for steps and other status
- Integrate second Bluetooth module
- Enable inter-pet communication

###Hardware:
- Create updated schematics
- Begin planning final Shield layout
- Begin running the numbers for power consumption so we can order the battery
- Figure out what extra parts are needed so we can place an order

More to be added.

HISTORY:
=====
1/12/2014 0.5
- Integrated Bluetooth Serial Module. Works great from smartphone, not so much via PC.
- Updated README.

1/3/2015 0.4
- Added more comments to code. Finally implemented drawing sprites from Flash memory. **Massive speed increases.Smooth animations now possible.**
- Created loading and sample pet animations. Working on tools to automate animation creation.
- Integrated EEPROM and wrote driver code for it.

12/26/14 0.3
- **Refactored entire code base. Project now requires Visual Studio 2013, Arduino 1.5.x and the Visual Micro plugin.**
- Added comments to code. 
- Integrated ClearBMP and many other library functions.