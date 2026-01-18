# atari-emu

`Atari ST` emulators in one portable package

![Hatari](https://raw.githubusercontent.com/Kochise/atari-emu/master/IMGS/hatari.png)

**DISK** : floppy disk images, hard disk images, utilities and programs sorted thematically<br>
**EMUL** : various emulators, listed below<br>
**ROMS** : bios images to run the emulators<br>

| **EMUL**			| [ARAnyM]						| [FalconXP]	| [Gemulator]	| [Hatari]		| [SainT]		| [Steem.SSE]	|
| :--				| :-:							| :-:			| :-:			| :-:			| :-:			| :-:			|
| **Version**		| 1.1.0							| 4.04			| 9.00 CE		| 2.6.1			| 2.40			| 4.2.0 R4		|
| **Date**			| 2019/04/14					| 2023/03/08	| 2008/11/30	| 2025/08/15	| 2015/12/12	| 2025/09/21	|
| **Release mouse**	| LShift + LCtrl + LAlt + Esc	| (borders)		| F11			| (borders)		| 				| F11			|
| **Screenshot**	| PrintScreen					| (host)		| 				| 				| 				| 				|
| **Fullscreen**	| ScrollLock					| (host)		| Alt + Enter	| F11			| 				| 				|
| **Main menu**		| Pause							| (Tray menu)	| Ctrl + F11	| F12			| F12			| (button)		|
| **Pause**			| (Main menu)					| 				| 				| Pause			| (Main menu)	| F12			|
| **Debug**			| LAlt + Pause					| 				| F12			| AltGr + Pause	| (Main menu)	| 				|
| **(Cold) Reset**	| (LShift +) LCtrl + Pause		| 				| Ctrl+F12		| AltGr + (c)/r	| (Shift +) F11	| 				|
| **Help**			| 								| F11			| 				| PrintSc		| PrintSc		| PageUp		|
| **Undo**			| 								| F12			| 				| ScrollBrk		| ScrollBrk		| PageDown		|
| **Border**		| 								| 				| 				| AltGr + b		| 				| 				|
| **Sound**			| AltGr + s						| 				| 				| AltGr + s		| 				| 				|
| **Quit**			| LShift + Pause				| (Tray menu)	| 				| AltGr + q		| 				| 				|

[ARAnyM]: https://github.com/aranym/aranym/releases
[FalconXP]: https://falcon.xp
[Gemulator]: http://www.emulators.com/download.htm#ATARIST
[Hatari]: https://www.hatari-emu.org/
[SainT]: http://leonard.oxg.free.fr/SainT/saint.html
[Steem.SSE]: https://sourceforge.net/projects/steemsse/files/

Providing "ready to use settings" is tricky because none of these emulators support relative path, hence I cannot prepare INI files linking to the provided folder/image/bios of the repository. Installation instructions are given below.

## ARAnyM

Mostly a desktop emulator to run `GEM` application and newer replacement desktop (like `Teradesk`). Can access the host network, but cannot map the host drives.

**INST** :

* Launch

* Configure each section (could be long)

## FalconXP Player

Atari application interpreter that runs in the background. Allow to execute them like native applications.

Applications are run as fast as the host can, emulating different CPU (68000 to 68060, FPU and DSP) yet with no cycle accuracy at all.

Drive letters are mapped to host drives or folders. Drag and drop, copy and paste work between host system and interpreted applications.

Hardware accesses may cause the interpreter to crash, don't try running drivers and ST games.

**INST** :

* Launch

* A tray icon allows to access parameters and loaded accessories
* Standard Atari application extensions (see below) are registered

Application extensions: ACC, APP, CPX, PRG, GTP, TOS, TTP

* A few desktop icons are created as shortcuts to screen resolution (see below)

Screen resolution shortcuts: ST LOW, ST MED, ST HIGH, 256, 65K, 16M, 32B

* Double click on an Atari application to run it through the interpreter using default settings

ACC and CPX are added to the tray icon

* Drag an Atari application to a screen resolution shortcut to run it in the desired colour mode
* Drag a disk image (see below) to a screen resolution shortcut to open it in the desired colour mode

Disk image format: ST/IMA/IMG/DSK, MSA, DIM, STX, IPF/CTR, RAW/HFE/SCP, STT/STG/STW, XFD/SD/DD

The `Player` version cannot create disk image and debug application.

## Gemulator

Fastest emulator out there, emulate as fast as your `CPU` can, good for `GEM` programs, not suited for games.

Can map the `Windows`' partitions to the emulated hard disks, hence see `C:\` as... `c:`. Great and exclusive feature, very convenient to share content and ease backup, hence makes it the true real `Atari` daily driver, as you can also support 800x600, 1024x768, 1280x1024, and even 1600x1200 screen resolution (with `VGAWIN.PRG`).

http://www.emulators.com/gemul8r.htm

**INST** :

* Launch

* Select "Atari ST"
* Configure "Options" menu (ROM, disk, VM : in that order)
* Click "Restart"

**BOOT** :

* Copy the `C_S000` folder in `C:\`
* Select to boot on `C:` (disk options)
* Restart
* Quickly press "5" as it boots to change desktop size

## Hatari
Good emulator, with pretty accurate `Falcon030` emulation, can emulate up to 1000% of the original speed (but it gets jerky).

**INST** :

* Launch

* Press `F12`
* Configure each section (could be long)

* Click "Reset machine"
* Click "OK"

## SainT
Accurate emulator with little tweaking options, yet tuned to run demos normally not runable on emulator.

**INST** :

* Launch

* Press "Options"
* Configure each section
* Add a ROM image (prefered 1.00)
* Select it
* Click "Set Active"

* Press "Disk Menu"
* Select the floppy disk image

* Then click "Run" (upper right corner)

## Steem.SSE
Currently the most up-to-date emulator, with an impressive set of hardware tweaks.

**INST** :

* Launch

* In the upper right corner...
* Configure each section (could be long)

* In the upper left corner...
* Right-click the red "Power" button (cold reset)
* Left-click the yellow "Play" button (run)
* Press `F11` to switch mouse control to the emulator
