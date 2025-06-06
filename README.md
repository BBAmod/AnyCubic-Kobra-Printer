# Based on ShadowFW, I do some changes to fit my new extruder head and pcba bug fix
* PCBA bug fix from this https://www.reddit.com/r/anycubic/comments/ynvbj0/comment/jkzzjzt/<br />
  and change   <br />
  X_SLAVE_ADDRESS 3 //default was 0<br />
  E0_SLAVE_ADDRESS 0 //default was 3<br />
    //#define STEALTHCHOP_E //without this changes linear advance will cause the extruder motor stop
* new extruder head help to reduce X axis weight, the stp 3D model you can find in https://www.thingiverse.com/thing:7011653 
* some speed and print aera changes, you can search "laya"in Configuration.h, I marked all the changes with laya in the file
* ~~Increased the homing speed (by 50%)~~
* ~~Increased the probes from 2 to 3~~
- - - -
 
 # AnyCubic Kobra Printer Firmware - ShadowFW

This is a modified version of the OEM firmware.

- - - -

### This is part of an optional UI rewrite.
Click the link below for more details on updating the printer UI.<br />
* https://github.com/George-Corrigan/AnyCubic-Kobra-Plus-Max-UI-ShadowUI

- - - -

Below is a list of versions and changes made:

#### v1.1
* Fixed a critical bug with resuming print after a power loss.

Note: Anycubic decided to leave a bug in their "released" source code that does not exist in their production source code.


#### v1.0
* Changed auto-leving and probing settings:
   * Increased the probing grid from 3x3 to 5x5
   * Increased the probes from 2 to 3
   * Increased the temp from 120/60 to 200/70
* Change startup beeps to a single beep.
* Changed z-offset step increment from 0.05 to 0.01
* Enabled linear advance support
* Increased the filament unload speed (by 100%)
* Increased the homing speed (by 50%)

- - - -

## Installing ShadowFW printer firmware

1. Download the latest release.

2. Locate the “firmware.bin” file copy to the root of a blank SD card.

3. Turn off the printer and insert the SD card in the printer (upside down, with the metal contacts up).

4. Remove any USB cable attached to the printer.

5. Turn on the printer and wait the a series of beeps, then the normal UI will load.

6. Remove the SD card.


## Reverting to original factory printer firmware

Note: This is to go back to factory defaults if you do not wish to continue using ShadowUI.

1. Go to AnyCubic’s website firmware page [https://www.anycubic.com/pages/firmware-software]. Select your printer series on the left. Select your model on the right. You should see latest firmware version and a link to download it.

2. Locate the “firmware.bin” file copy to the root of a blank SD card.

3. Turn off the printer and insert the SD card in the printer (upside down, with the metal contacts up).

4. Remove any USB cable attached to the printer.

5. Turn on the printer and wait the a series of beeps, then the normal UI will load.

6. Remove the SD card.
