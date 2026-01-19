                          SELTOS.PRG for Atari ST computers

  Purpose of this program is making possible work with diverse versions of TOS without changing (EP)ROM's in machine. 
 1 MB of RAM is minimum requirement for use, because used TOS is placed in RAM, and it decreases available RAM for 192-256KB.

  Program supports following TOS versions:
TOS 1.00 US, UK and German
TOS 1.04 US, UK and German
TOS 2.06 US, UK , German, Swedish  and UK 'Supertos' .

   Instructions for SELTOS: 
  Program may be used in 3 ways:
1: Drag and drop - just drag ROM image file on SELTOS.PRG. Program will outprint TOS version, and install it. This works only with TOS 2.06.

2: If install from older TOS version, user may rename program to SELTOS.TTP, and after start enter desired ROM image name.
If image has extension IMG, it is not necessary to type in extension.

3: Select TOS from list - ROM images must be placed in same directory with program, and must have extension IMG . Name is irrelevant.
After program start, it scans all files with IMG extension, and looks for supported versions. If it is found, program outprints version on screen, with number for further selection.
  Max. 9 files of supported TOS versions can be handled. 
After it, user should select which version want install with keys 1-9. Then program loads selected TOS, makes relocation and restarts computer. Installed version survives reset.

 If you want uninstall it, press shift+alt+control+delete keys simultaneous. It works not with TOS 1.00. Of course turning off machine also 'uninstalls'.


  If user wants install other (or same) TOS, when one is already installed, program will warn about it, but allows installation. However, it is not recommended because previous installation remains in RAM, and decreases free memory.

  Instructions for RAMTOS:
 On request I made version which loads straight first supported TOS image in current DIR. It is RAMTOS7.PRG, and everything above except usage 
stays for. Of course you may run it from AUTO folder too.




  How to get ROM-TOS images:
There is small program supplied with emulator Pacifist named 'DUMP_ROM.PRG', with which you may record ROM-TOS to disk. 
There are also many TOS versions downloadable on the WEB, but you should look self about it, 
because copyright status is not exactly clear for me. E.g. if I have Atari with German TOS 1.04, may I use US TOS 1.04 ?

  Note: ROM_TOS image files must have adequate length:
TOS 1.00 and 1.04: 192KB or 196608 bytes.
TOS 2.06: 256KB or 262144 bytes.



  This program is freeware.


Pera Putnik,  March 13. 1999.
   some additions in 2006, 2008

  pp@ppest.org





 
