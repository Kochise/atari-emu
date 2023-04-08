# atari-emu
`Atari ST` emulators in one portable package

![Hatari](https://raw.githubusercontent.com/Kochise/atari-emu/master/IMGS/hatari.png)

**DISK** : floppy disk images, hard disk images, utilities and programs sorted thematically<br>
**EMUL** : various emulators, listed below<br>
**ROMS** : bios images to run the emulators<br>

| **EMUL**			| [ARAnyM]						| [Gemulator]	| [Hatari]		| [SainT]		| [Steem.SSE]	| [BigPEmu]		| [VirtualJaguar]	|
| :--				| :-:							| :-:			| :-:			| :-:			| :-:			| :-:			| :-:				|
| **Version**		| 1.1.0							| 9.00 CE		| 2.4.1			| 2.40			| 4.1.2 R5		| 1.054			| 2.1.2				|
| **Date**			| 2019/04/14					| 2008/11/30	| 2022/08/03	| 2015/12/12	| 2022/10/23	| 2023/03/07	| 2014/10/10		|
| **Release mouse**	| LShift + LCtrl + LAlt + Esc	| F11			| (borders)		| 				| F11			| 				| 					|
| **Screenshot**	| PrintScreen					| 				| 				| 				| 				| 				| 					|
| **Fullscreen**	| ScrollLock					| Alt + Enter	| F11			| 				| 				| 				| 					|
| **Main menu**		| Pause							| Ctrl + F11	| F12			| F12			| (button)		| 				| 					|
| **Pause**			| (Main menu)					| 				| Pause			| (Main menu)	| F12			| 				| 					|
| **Debug**			| LAlt + Pause					| F12			| AltGr + Pause	| (Main menu)	| 				| 				| 					|
| **(Cold) Reset**	| (LShift +) LCtrl + Pause		| Ctrl+F12		| AltGr + (c)/r	| (Shift +) F11	| 				| 				| 					| 
| **Help**			| 								| 				| PrintSc		| PrintSc		| PageUp		| 				| 					| 
| **Undo**			| 								| 				| ScrollBrk		| ScrollBrk		| PageDown		| 				| 					| 
| **Border**		| 								| 				| AltGr + b		| 				| 				| 				| 					|
| **Sound**			| AltGr + s						| 				| AltGr + s		| 				| 				| 				| 					|
| **Quit**			| LShift + Pause				| 				| AltGr + q		| 				| 				| 				| 					|

[ARAnyM]: https://github.com/aranym/aranym/releases
[Gemulator]: http://www.emulators.com/download.htm#ATARIST
[Hatari]: https://download.tuxfamily.org/hatari/
[SainT]: http://leonard.oxg.free.fr/SainT/saint.html
[Steem.SSE]: https://sourceforge.net/projects/steemsse/files/
[BigPEmu]: http://richwhitehouse.com/jaguar/
[VirtualJaguar]: https://icculus.org/virtualjaguar/

Providing "ready to use settings" is tricky because none of these emulators support relative path, hence I cannot prepare INI files linking to the provided folder/image/bios of the repository. Installation instructions are given below.

## ARAnyM
Mostly a desktop emulator to run `GEM` application and newer replacement desktop (like `Teradesk`). Can access the host network, but cannot map the host drives.

**INST** :
* Launch<br>

* Configure each section (could be long)<br>

## Gemulator
Fastest emulator out there, emulate as fast as your `CPU` can, good for `GEM` programs, not suited for games.

Can map the `Windows`' partitions to the emulated hard disks, hence see `C:\` as... `c:`. Great and exclusive feature, very convenient to share content and ease backup, hence makes it the true real `Atari` daily driver, as you can also support 800x600, 1024x768, 1280x1024, and even 1600x1200 screen resolution (with `VGAWIN.PRG`).

http://www.emulators.com/gemul8r.htm

**INST** :
* Launch<br>

* Select "Atari ST"<br>
* Configure "Options" menu (ROM, disk, VM : in that order)<br>
* Click "Restart"<br>

**BOOT** :
* Copy the `C_S000` folder in `C:\`<br>
* Select to boot on `C:` (disk options)
* Restart<br>
* Quickly press "5" as it boots to change desktop size<br>

## Hatari
Good emulator, with pretty accurate `Falcon030` emulation, can emulate up to 1000% of the original speed (but it gets jerky).

**INST** :
* Launch<br>

* Press `F12`<br>
* Configure each section (could be long)<br>

* Click "Reset machine"<br>
* Click "OK"<br>

## SainT
Accurate emulator with little tweaking options, yet tuned to run demos normally not runable on emulator.

**INST** :
* Launch<br>

* Press "Options"<br>
* Configure each section<br>
* Add a ROM image (prefered 1.00)<br>
* Select it<br>
* Click "Set Active"<br>

* Press "Disk Menu"<br>
* Select the floppy disk image<br>

* Then click "Run" (upper right corner)<br>

## Steem.SSE
Currently the most up-to-date emulator, with an impressive set of hardware tweaks.

**INST** :
* Launch<br>

* In the upper right corner...<br>
* Configure each section (could be long)<br>

* In the upper left corner...<br>
* Right-click the red "Power" button (cold reset)<br>
* Left-click the yellow "Play" button (run)<br>
* Press `F11` to switch mouse control to the emulator<br>

## BigPEmu
The `Jaguar` emulator inside the new [VCS] console, released as a stand-alone program.

Best emulator so far, because recent and still under development by [Rich Whitehouse].

[VCS]: https://atari.com/collections/atari-vcs
[Rich Whitehouse]: https://twitter.com/DickWhitehouse

**INST** :
* Launch<br>

* Follow menu<br>

## VirtualJaguar
While originally targeted to emulate the `Jaguar`, aims to emulate the unreleased `Microbox` as well.

**INST** :
* Copy `ROMs` in `atari-emu/EMUL/VirtualJaguar/software` (`j64` in `zip` files)<br>
* Launch<br>

* Select "Jaguar/Configure"...<br>
* In "General", change path for "EEPROMs" and "Software"<br>
* In "Controllers", change keys mapping<br>

* Select "Jaguar/Insert Cartidge..."<br>
* Click the wanted `ROM`<br>
* Click the "Insert cartidge" icon in the lower right corner<br>
