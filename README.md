# atari-emu
Atari ST emulators in one portable package

![Hatari](https://raw.githubusercontent.com/Kochise/atari-emu/master/IMGS/hatari.png)

DISK : floppy disk images, hard disk images, utilities and programs sorted thematically<br>
EMUL : various emulators, listed below<br>
ROMS : bios images to run the emulators<br>

| EMUL				| Version	| Release mouse			| Fullscreen	| Main menu	| Pause			| Debug			| (Cold) Reset			| Help		| Undo		|
| :--				| :-:		| :-:					| :-:			| :-:		| :-:			| :-:			| :-:					| :-:		| :-:		|
| [ARAnyM]			| 1.10		| LShift+LCtrl+LAlt+Esc	| ScrollLock	| Pause		| (menu)		| LAlt+Pause	| (LShift+)LCtrl+Pause	| n/a		| n/a		|
| [Gemulator]		| 9.00 CE	| F11					| Alt+Enter		| Ctrl+F11	| n/a			| F12			| Ctrl+F12				| n/a		| n/a		|
| [Hatari]			| 2.3.1		| (borders)				| F11			| F12		| (menu)		| n/a			| n/a					| n/a		| n/a		|
| [SainT]			| 2.40		| n/a					| n/a			| F12		| (menu)		| (menu)		| (Shift+)F11			| PrintScr	| ScrollBrk	|
| [Steem.SSE]		| 4.1.2		| F11					| n/a			| (button)	| n/a			| n/a			| n/a					| PageUp	| PageDown	|
| [VirtualJaguar]	| 2.1.2		| n/a					| n/a			| n/a		| n/a			| n/a			| n/a					| n/a		| n/a		|

[ARAnyM]: https://github.com/aranym/aranym/releases
[Gemulator]: http://www.emulators.com/download.htm#ATARIST
[Hatari]: https://download.tuxfamily.org/hatari/
[SainT]: http://leonard.oxg.free.fr/SainT/saint.html
[Steem.SSE]: https://sourceforge.net/projects/steemsse/files/
[VirtualJaguar]: https://icculus.org/virtualjaguar/

Providing ready to use settings is tricky because none of these emulators support relative path, hence I cannot prepare INI files linking to the provided folder/image/bios of the repository. Installation instructions are given below.

## ARAnyM
Mostly a desktop emulator to run GEM application and newer replacement desktop (like Teradesk). Can access the host network, but cannot map the host drive.

## Gemulator
Fastest emulator out there, emulate as fast as your CPU can, good for GEM programs, not suited for games.

Can map the Windows' partitions to the emulated hard disks, hence see C:\ as... c:. Great and exclusive feature, very convenient to share content and ease backup, hence makes it the true real Atari daily driver, as you can also support 800x600, 1024x768, 1280x1024, and even 1600x1200 screen resolution (with VGAWIN.PRG).

http://www.emulators.com/gemul8r.htm

INST : Launch, select "Atari ST", configure "Options" menu (ROM, disk, VM, in that order), click "Restart"<br>
BOOT : Copy the C_S000 folder in C:\, select to boot on C: (disk options), restart, quickly press 5 as it boots to change desktop size<br>

## Hatari
Good emulator, with pretty accurate Falcon030 emulation, can emulate up to 1000% of the original speed (but it gets jerky).

INST : Launch, press F12, configure each section<br>

## SainT
Accurate emulator with little tweaking options, yet tuned to run demos normally not runable on emulator.

## Steem.SSE
Currently the most up-to-date emulator, with an impressive set of hardware tweaks.

## VirtualJaguar
While originally targeted to emulate the Jaguar, aims to emulate the unreleased Microbox as well.
