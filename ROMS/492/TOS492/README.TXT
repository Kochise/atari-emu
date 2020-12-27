For all Falcon030 Developers...
This is a little kit to make a FalconTOS 4.01 boot floppy.  With this
you will be able to boot a Falcon with a ROM image from floppy and
have the newest version of TOS running.  The only thing missing from
this archive is a utility called "CAUTO.PRG" that you would put in
the AUTO folder on A: _AFTER_ SHDRIVER.PRG  CAUTO.PRG is a utility
(from Atari available on GEnie, CIS, etc.; I just don't have it at
home here) that when run in the AUTO folder on A: will continue the
boot sequence to the AUTO folder of C:

To make a boot disk, simply run MKBOOT.PRG with a blank floppy in A:
and use "BOOTSEC.SYS A" for the arguments.  Copy TOS.IMG to the
floppy, create an AUTO folder, and copy SHDRIVER.PRG (renamed
SHDRIVER.SYS) into the AUTO folder.  If you have CAUTO.PRG, copy that
into the AUTO folder next.

If you have any problems with this installation, read the GEMDOS
documentation that explains how bootable sectors on floppies work,
then using a sector editor, you can hand-make your own bootable
floppy.

Have fun,

 -Bill

P.S. Things that I know that are fixed in this TOS... 1) Fill
patterns are no longer stuck on USER DEFINED.  2) AES is better
behaved regarding wind_calc, wind_get, etc.  3) Video timings have
been cleaned up a bit to give better color, especially in NTSC.
4) Keyboard mapping (should) work.  
There are definitely many more fixes, but I'm really tired and my
foot hurts because a 258 pound Mitsubishi 37" Diamond Scan fell on it
a few hours ago.
