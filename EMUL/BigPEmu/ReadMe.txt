Title: BigPEmu
Version: 1.054
Author: Rich Whitehouse
Website: https://www.richwhitehouse.com/jaguar/
Description: An Atari Jaguar emulator, featuring compatibility with every title in the Jaguar's retail cartridge library. See below for copyright and licensing information.


Patreon: /richwhitehouse
https://patreon.com/richwhitehouse

Twitter: @DickWhitehouse
https://twitter.com/DickWhitehouse

Please consider showing your support!


Release Notes
-------------
Version 1.054
 - Made some adjustments to the refresh rate warning logic introduced in the last version. The warning is inhibited when VSync is disabled (no, don't do it!), and an option to disable the warning has been added.

Version 1.053
 - VLM's CD+G support is now working.
 - Refresh frequency can now be specified when selecting a display mode. (previously, the highest available was auto-selected)
 - The program will now warn you if you're using a native display refresh rate which is lower than the Jaguar's current display frequency. (50Hz for PAL, 60Hz for NTSC)
 - There's now a warning under the Mount Images menu telling users to avoid setting a cartridge image with a disc image if the two were not designed to run together. Depending on the CD BIOS (and if one is set at all), this can cause a wide variety of problems, so don't do it.
 - Fixed a Blitter issue which was introduced several versions ago. The issue could result in crashes, especially when running Fight for Life.
 - Fixed a problem with SUBQMOD, although there are no known instances where this fix changes any behavior in the retail library.
 - Fixed more unmapped read behavior which was preventing some homebrew from running correctly.
 - Fixed a Blitter negative base address issue.
 - Various other minor bug fixes. Thanks to all of the people who submitted bug reports through the web site!

Version 1.052
 - Added a "Sample Scale" option for the System Audio Disc.
 - I heard you like minor timing fixes, so I made some minor timing fixes to the minor timing fixes.

Version 1.051
 - Minor timing fixes.

Version 1.05
 - Jaguar CD support! This encompasses a whole bunch of new functionality. Thank you to Mycah Mattox for donating the Jaguar CD hardware, this hardware was essential for my research.
 - Added -nodevicechange and -fulldevrefresh as command line options.
 - Added -forceloadaddr and -forcerunaddr as command line options.
 - Added -setcfgprop and -setcfgpropcat as command line options.
 - Added -audiocapindex as a command line option, can be used when launching *VirtualDisc_SystemAudio as the ROM image via command line.
 - Added -physdevindex as a command line option, can be used when launching *VirtualDisc_SystemPath as the ROM image via command line.
 - Added another DirectInput workaround which may help prevent stuck analog controls on some machines. Thanks to neurocrash for spending a lot of time running tests for me on a machine where this was an issue!
 - Fixed a problem with depth writes in 32-bit mode, thanks to 42Bastian for providing a test case.
 - Fixed an issue with word strobes not being correctly scheduled until a write to SMODE/SCLK. (only relevant when running with no boot ROM set)
 - Fixed a timing problem with EEPROM reads.
 - Fixed scaling on monitors with a non-standard DPI.
 - When adding a new individual input trigger in the binds menu, the prompt mode now defaults to "specify hold and button" instead of just "specify button". (in line with binding all inputs via "Set All")
 - Lots of additional core emulation fixes which came along with CD support.
 - Implemented some intentional redundancy in the OpenGL video plugin's state management, to guard against things like Discord's video capture poking around in the context without cleaning up after itself.
 - With CD support wrapped up, I'm taking a short hiatus! I need to recouperate, generate some income, and prepare for the next phase of my cancer treatment. Please don't inundate me with fix/feature requests via e-mail/Discord/etc. during this time. Instead, I ask that you use the less invasive Bug Report form on the web site. Thank you for your consideration!

Version 1.04
 - Added -devmode as a command line option. Don't get too excited, this isn't opening up full developer build functionality just yet. (and the option will be auto-enabled in proper developer builds) It does enable a developer menu, though, and some additional developer-oriented key binds.
 - Added a "Local EEPROM" option to the new developer menu at CJ Reboot's request.
 - Added a "Save Window Position" option to the new developer menu at CJ Reboot's request.
 - Added an "Always on Top" option to the new developer menu at CJ Reboot's request.
 - Added a new set of developer input binds, which are only accessible when using -devmode. Most of these new binds were added at 42Bastian's request.
 - Now picking through individual device/state changes instead of just refreshing all input devices when a DEVNODES_CHANGED-type message is received. Fixes input disruptions on machines where something is causing DEVNODES_CHANGED spam.
 - Fixed a problem when recording a movie with the "Increment on Save" setting enabled. Thanks to Doug Engel for finding this bug!
 - Fixed a crash when entering the Zero 5 sound options screen.
 - Every issue which was submitted through the Bug Report form on the website has now been resolved. Thanks to everyone who contributed! If you submitted something which still isn't resolved in this build, let me know.

Version 1.03
 - Fix for some emulator state overrides not sticking when launching software via command line.

Version 1.02
 - Added interlaced display support. This is definitely not working the way it works on hardware, but it addresses the test case that Doug Engel was kind enough to send over.
 - Added a "force horizontal overscan" option.
 - Added an option to force Jaguar GD emulation, which enables bank switching even in ROM images smaller than 6MB.
 - Added -localdata as a command line option.
 - Changed the first visible column default. (applies to both NTSC and PAL)
 - Fix for a 16-bit DAC write bug. (fixes one-sided audio in Super Burnout)
 - Fix for a problem on the Blitter's SRCSHADE data path. Thanks to Doug Engel for reporting the BattleSphere bug which led to this fix. 
 - Fix for OP sometimes running over HDE, thanks to CJ Reboot for reporting the associated bug.
 - Fix for input bindings sometimes not being picked up from per-game config files. This bug could also potentially cause game-specific bindings to "stick" and infect the global config bindings.

Version 1.01
 - Added variable-sized EEPROM support, thanks to CJ Reboot for providing some test cases. 512 byte and 2KB EEPROM settings are picked up from the MRQ file if it exists, otherwise any number of EEPROM address bits can be specified via the -eeprombits command line option.
 - Added Jaguar GD bank switching support.
 - Fix for exception-handled M68K reads in unmapped address space.
 - Fix for Blitter not correctly wrapping cyan/red on add when ADDDSEL is set, thanks to Doug Engel for reporting the BattleSphere bug which led to this fix.
 - Fix for Blitter not starting on the correct bit when block width is less than pixel size and phrase mode is not enabled, thanks to Doug Engel for reporting the BattleSphere bug which led to this fix.
 - Going in for a major surgery tomorrow (2022/12/06), so expect that I'll be mostly unavailable and that there won't be any emulator updates for at least a couple of weeks.

Version 1.00
 - First public release!


Troubleshooting
---------------
If you're encountering problems with BigPEmu after a successful first use, the first thing you should try is clearing out your configuration. You can do this by deleting the BigPEmuConfig.bigpcfg in your %APPDATA%\BigPEmu folder. (this is usually somewhere like C:\Users\YourUsername\AppData\Roaming\BigPEmu)

If you have a different problem or your problem persists after deleting your configuration file, check the website for additional help and information.


Copyright, Permissions, and Special Thanks
------------------------------------------
BigPEmu Copyright (C) 2022 Rich Whitehouse

Special thanks to everyone at Digital Eclipse, especially Mike Mika and Stephen Frost! These guys helped fund the development of this emulator, and helped make sure I'd be able to release it publicly. In the video game industry, that's a real gesture of love.

Extra special thanks to Mark Sowden (hogsy) for being a friend and long-time supporter of my work.

Even more special thanks to Mycah Mattox for donating a Jaguar CD unit!

Those Who Bare Fangs at God:
 - Mark Sowden (hogsy)
 - John Perez (neurocrash)
 - ZoeB
 - B.J. West
May your oppressors suffer all the torments this world has to offer, my friends.

Thank you to my daughter, Alaura, for brightening my life from the moment of her birth.

Thank you to my wife and my good friends (Ruth, Kirt, Joseph, Boris, Jake) for helping me through (the remains of) my life as I suffer with cancer.

As with all of the free software that I write, I accept no liability and offer no warranty. By using this software, you agree that you are solely responsible for any form of harm which may result from the use of this software.

I forbid the use of this software for any form of private financial gain. You may not distribute this software without the inclusion of this document in unmodified form, and you may not distribute modified versions of the main BigPEmu executable file. Any exemption from these terms requires my written permission.

For a list of third party software included in BigPEmu and the corresponding license/copyright information, please see the Data/ThirdParty/Licenses directory. I've also auto-converted and included a few shaders written by Hyllian under the MIT license, see the relevant shader files in Data/ScreenEffects for the original license/copyright information.

BigPEmu: The World's Prefurred Large Pussycat Emulator

I'd say I'm going to hell, but I'm already there!
