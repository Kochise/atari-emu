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

<details>
<summary>Application extensions: ACC/CPX, APP/PRG/GTP, TOS/TTP</summary>

```
ACC: ACCessory
CPX: Control Panel eXtension

APP: APPlication (GEM)
PRG: PRoGram (GEM)
GTP: Gem Taking Parameter (GEM)

TOS: Text Output to Screen (VT52)
TTP: Tos Taking Parameter (VT52)
```

</details>

* A few desktop icons are created as shortcuts to screen resolution (see below)

<details>
<summary>Screen resolution shortcuts: ST LOW/ST MED/ST HIGH, TT LOW/TT MED/TT HIGH, 256P/256C, 65K, 16M, 32B</summary>

```
ST LOW: 320x200, 16 colours out of 512(ST)/4096(STe), 4 bitplanes
ST MED: 640x200, 4 colours out of 512(ST)/4096(STe), 2 bitplanes
ST HIGH: 640x400, monochrome (black and white), 1 bitplane

TT LOW: 320x480, 256 colours out of 4096, 4 bitplanes
TT MED: 640x480, 16 colours out of 4096, 2 bitplanes
TT HIGH: 1280x960, monochrome (black and white), 1 bitplane

256P: 256 colours out of 262144, 8 bitplanes (planar)
256C: 256 colours out of 262144, chunky (1 byte per pixel)

65K: 65535 colours, 16 bits RGB (565)
16M: 16777216 colours, 24 bits RGB (888)
32B: 16777216 colours, 32 bits ARGB (8888)
```

</details>

* Double click on an Atari application to run it through the interpreter using default settings

ACC and CPX are added to the tray icon

* Drag an Atari application to a screen resolution shortcut to run it in the desired colour mode
* Drag a disk image (see below) to a screen resolution shortcut to open it in the desired colour mode

<details>
<summary>Disk image format: ST, IMA/IMG/DSK, MSA, IMD/DIM, BIN/STC (cartridge), STX, IPF/CTR, RAW/SCP/HFE, STT/STG/STW</summary>

```
ST: created originally for the PacifiST emulator

IMG: created by the Copy II PC Option board (generic suffixes for IBM FM/MFM raw sectors)

MSA: created by the compression program Magic Shadow Archiver

IMD: ImageDisk image (IBM sectors)
DIM: created by the well-known Atari copy program "FastCopy Pro"

STX: created by the PASTI initiative (Atari ST Imaging & Preservation Tools)

IPF: created by the Software Preservation Society (universal 'golden image' format)
CTR: created by the Software Preservation Society

RAW created by the KryoFLux board (raw bitcells, 1 revolution per track)
SCP: created by the SuperCard Pro board (raw flux, multiple revolutions per track)
HFE: created by the HxC Floppy Emulator (HxC2001.com project)

STT: created for Steem Engine emulator
STG: ST Gost format, created for Steem SSE
STW: ST Write format, created for Steem SSE
```

</details>

* Double click on an Atari picture (see below) to display it through the interpreter using default settings
* Drag an Atari picture (see below) to a screen resolution shortcut to display it in the desired colour mode

<details>
<summary>Atari picture format: ANM, CE1/CE2/CE3, GEM/IMG/XIMG, FLM, NEO/ANI, PAC, PC1/PC2/PC3/PI1/PI2/PI3, RGB, SEQ, TNY/TN1/TN2/TN3, UC1/UC2/UC3</summary>

```
CE1/CE2/CE3: ComputerEyes
GEM/IMG/XIMG: GEM and VDI native
FLM: Animatic Film
NEO/ANI: NEOchrome
PAC: STAD
PC1/PC2/PC3/PI1/PI2/PI3: Degas Elite
RGB: RGB intermediate
SEQ: Cyber Paint Sequence
TNY/TN1/TN2/TN3: Tiny
UC1/UC2/UC3: Imagic
```

</details>

Typically the last digit means '1' for ST LOW, '2' for ST MED and '3' for ST HIGH.

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
