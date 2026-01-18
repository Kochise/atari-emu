Replacement icons for TOS 2.06
------------------------------
The enclosed icon file (and desktop setup file) 
gives you loads of replacement icons for all sorts of files.
I've also done the time-consuming task of assigning icons to most 
of the general files you will encounter (text files, image files, 
CPX modules, ACCessories, programs etc.).
And since TOS 2.06 can save desktop window size/positions I've organized 
the windows in a way that I think most people will be happy with when 
they're opened.

Install these files on your system as a general, basic setup which you 
can add things to later if you wish (install programs, add extra icons
etc.)



Files enclosed
--------------
* DESKICON.RSC     -The icon file containing 81 icons
* ICONS.IMG        -A screenshot image of the icons
* NEWDESK.INF      -The desktop setup file with the icon assignments
* README.TXT       -This text file


Installation
------------
Assuming that you haven't already made a working setup on your computer 
(in which case, just back up DESKICON.RSC and NEWDESK.INF which you 
can revert to in case you don't like the setup presented here)...

1) Copy the enclosed DESKICON.RSC file to the root of your boot-drive.

2) Copy the enclosed NEWDESK.INF file to the root of your boot-drive.

 "root of the boot drive" meaning:

   "Boot Drive" = the startup drive where you keep your ACCessories,
                  AUTO programs etc.
                  This is usually drive C on a hard disk, or 
                  on a floppy-disk system it's the same disk you have
                  your ACCessories and AUTO programs on, 
                  inserted into the drive each time you start the computer.
 
   "Root" = not inside any folders. The same place as where your
            ACCessories are found on the boot drive/disk.

3) Reboot (restart) the computer. This will load all the icons and their
   file-assignments. 

4) You will see the desktop with the trash-can, hard disk drive C and 
   floppy disk drive A. 
   This might not correspond to your actual system, so go to 
   the "Options" menu, then select "Install Devices". 
   
   You will now end up with an icon on the desktop for all your available 
   hard disks, CD-ROM drives, floppy drives etc. 
   If there are any drives you don't want (i.e. floppy drive B which isn't 
   needed on a system without a second disk drive) you may click on that 
   device once (to select it), then go to the "Options" menu and 
   choose "Remove Desktop Icon".

5) Go to the "Options" menu, then select "Save Desktop" 
   to save your current desktop.



File assignments
----------------
To save you lots of time I've gone through the various file types you're 
most likely to encounter and assigned them to icons. 
This means that for example all files ending with ".JPG" will have the 
same icon, representing them as JPEG image files. 

Here's a complete list of the files I've assigned to the various icons 
("Install Icon" in the "Options" menu)... 


NAME          TYPE    DESCRIPTION
---------------------------------
AUTO          Folder  AUTO folder
*.APP         File    GEM Application    
*.APX         File    GEM Application (disabled)
*.PRG         File    GEM Program
*.PRX         File    GEM Program (disabled)
*.ACC         File    Accessory
*.ACX         File    Accessory (disabled)
*.GTP         File    GTP (GEM Takes Parameters) application
*.TTP         File    TOS (TOS Takes Parameters) application
*.TOS         File    TOS application
CPX           Folder  CPX module folder
*.CPX         File    CPX module
*.CPZ         File    CPX module (disabled)
*.SET         File    Setup file
*.INF         File    Info file
*.CFG         File    Configuration file
*.CNF         File    Configuration file
*.SYS         File    System file
*.BAK         File    Backup file
*.TXT         File    ASCII text file
*.DOC         File    1st Word/ASCII text file [* see note below]
READ*.*       File    Readme file (READ.ME, README.1ST, READ_ME.NOW etc.) 
CLIPBRD       Folder  Clipboard folder 
SCRAP.*       File    Clipboard file (SCRAP.TXT, SCRAP.IMG etc.)
*.HYP         File    ST-Guide Hypertext help-file
*.RSC         File    Resource file
*.IMG         File    IMG image file
*.GIF         File    GIF image file
*.JPG         File    JPG image file
*.GEM         File    GEM image file
*.PI1         File    PI1 image file
*.PI2         File    PI2 image file
*.PI3         File    PI3 image file
*.SND         File    SND audio sample file
*.AIF         File    AIFF audio sample file
*.HSN         File    HSN audio sample file
*.AVR         File    AVR audio sample file
*.SMP         File    SMP audio sample file
*.WAV         File    WAVE audio sample file
*.AU          File    AU audio sample file
*.FNT         File    GEM font
*.SPD         File    Speedo GDOS font
*.TTF         File    TrueType font
*.HTM         File    HTML (Hypertext Markup Language) file
*.LZH         File    LZH archive file
*.ZIP         File    ZIP archive file
*.ARC         File    ARC archive file 
*.ZOO         File    ZOO archive file


[*] The ".DOC" extension belongs to files created with the "1st word"
and "First Word Plus" word-processors, but since they can also
read/write plain ASCII text files people went on naming plain ASCII
files ".DOC" as well (instead of the more correct ".TXT" ending). 
The mentioned programs can read both formats just fine, but viewing a
".DOC" file which has been saved as a word-processor document gives all
sorts of strange formatting characters. 

To confuse the issue even more, "Microsoft Word" documents from PCs or
Macs share that same ".DOC" file-ending, but are of course totally
incompatible with the above. 

Anyway, since most ".DOC" files are actually plain ASCII text files
anyway I use the built-in viewer of TOS (or a replacement, such as
"GUCK" to display any text file, and for the occasional word-processor
formatted ".DOC" file I use "1st Word". 
Incidently, GUCK actually reads 1st Word/First Word Plus word-processor
documents, but without a standard font (you loose out on the
bold/italic/underlining etc.) 




The 12 Default icons
--------------------
Atari placed 12 basic icons representing files/programs/drives etc. in 
the TOS 2.06 ROM/EPROM chips. 
These icons are the bare necessities of representing files/programs/folders, 
the trash-can, drives etc. but with most files you get the same type of 
icons which isn't very exciting.

However, by adding a resource (.RSC) file named DESKICON.RSC 
these 12 icons are REPLACED with the first 12 icons of in that new file. 

Because those 12 icons represent anything that hasn't had an icon assigned 
to it, you should keep a copy of those same icons (in the same order) 
at the beginning of any DESKICON.RSC file.

If you really don't like those icons, finding them a total waste you 
can of course replace them with equivelant icons (i.e. a different 
trash-can icon for the trash-can etc.), but be sure to keep them in 
the same order. 

When ADDING an icon to the file, ADD IT TO THE END OF THE FILE. 
When REMOVING an icon from the file, PUT A NEW ICON IN ITS PLACE.

In case you get this wrong and the (i.e. text-file icon) which was 
previously the 22nd icon from the top now suddenly gets shifted 
to place number 21 because you removed an icon you might get text files 
to be displayed with a CPX module icon instead, and all assigned files 
after this icon will also be wrong.

The key to keep things working properly is to keep all the icons 
that are being used in the same order all the time.
It's always a good idea to work on a backup copy of the DESKICON.RSC file 
in case you mess things up.
You should also make a backup copy of the NEWDESK.INF file.

(In case you end up with a hard disk crash, a damaged floppy 
or plain and simply accidently delete one or both of these files you don't have 
to spend hours recreating your entire setup again!). 


Hallvard Tangeraas, 24-February 2002
