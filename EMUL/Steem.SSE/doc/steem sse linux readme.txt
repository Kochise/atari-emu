STEEM SSE README

This is about the Linux port of the legendary Atari ST emulator Steem.
Copy files to a folder of your choice and run steem32 or steem64.


****************
* Requirements *
****************

Steem SSE targets a Linux Ubuntu 20.04 distro, 32bit or 64bit.

The following in case you have trouble starting Steem SSE due to missing
libraries.
In some Linux distros you are informed of missing libraries only if you run
the program from the terminal.
Note that even if you don't need the feature, the library is necessary.
It's not optional like a dll in Windows AFAIK.
Beside those provided with the download, some other libraries could be
necessary. If it doesn't work, run from a terminal opened in Steem's folder,
and check the error messages, some libraries could be missing:

./steem32
or
./steem64


You can use apt-file to find which package you need to download based on
the error message:
sudo apt-get install apt-file
sudo apt-file update

usage:

apt-file search "..."


***************
* Limitations *
***************

Compared with the Windows build, it has fewer features. 
The Steem SSE Manual file applies to Steem SSE, the Windows build.
Here are some current limitations of Steem SSE.

XSteem is developed on a VMware virtual machine, by someone who doesn't
know the system very well. There's only one dev for Steem SSE.

Debugger: no support
Pasti images (STX): no support
RTF files: no support
GUI keyboard control: no support
Status bar: no support
ST Aspect Ratio: no support
VSync: no support
CRT emu: no support
MIDI options: no support
Emulation thread: no support

Despite those limitations, Steem SSE is still a competitive emulator on Linux.
Its emulation core is the same as Steem SSE for Windows and it's pretty good.


**************************************
* CAPS/SPS images (CTR, IPF) support *
**************************************

For CTR/IPF disk image support, you may need to install a library, capsimage.

You can use the library included in the download or compile it yourself
(read 3rdparty/caps_linux/SSE.txt in the source).
This should be placed in a shared library directory (usr/lib/...) and renamed
libcapsimage.so.5, or an alias should be created.

Open a terminal where the files were copied and type:

sudo cp libcapsimage.so.5.1 usr/lib

or

sudo cp libcapsimage.so.5.1 usr/lib/i386-linux-gnu

Open a terminal in the library folder and type:

sudo mv libcapsimage.so.5.1 libcapsimage.so.5

or:

sudo ln -s libcapsimage.so.5.1 libcapsimage.so.5

If you also want to compile steem with the library, the same files should
be copied to:

usr/local/lib

Don't ask me why. It would be possible to link statically, but it adds more than
400K to the executable, and it defeats the purpose of a library.

For the 64bit build, the library should be copied to /usr/lib/x86_64-linux-gnu
instead:

sudo cp libcapsimage.so.5.1 /usr/lib/x86_64-linux-gnu

Then from that folder:

sudo ln -s libcapsimage.so.5.1 libcapsimage.so.5


***************
* RAR support *
***************

Steem SSE can read RAR files, provided unrar is installed.
Steem uses the command-line "unrar", there's no library like unrar.dll or
unrar64.dll in Windows.
The following command in the terminal should install unrar:

sudo apt-get install unrar


***************
* 7z support *
***************

Steem SSE can read 7z files, provided 7za is installed.
Steem uses the command-line "7za", there's no library like archiveaccess.dll in
Windows.
The following command in the terminal should install 7za:

sudo apt install p7zip-full


************
* Shortcut *
************

Edit the path in steem32.desktop or steem64.desktop, replacing

/home/osboxes/Documents/ST/X-build/output/

with your path to Steem SSE, then

cp steem32.desktop ~/.local/share/applications/

or

cp steem64.desktop ~/.local/share/applications/


To check integrity:

sudo desktop-file-install steem32.desktop

or

sudo desktop-file-install steem64.desktop


*******
* doc *
*******

Some doc files are in RTF format, sorry about that


**********************
* Steem SSE Web site *
**********************

https://sourceforge.net/projects/steemsse/
