CLA-FSMedit v1.2                                             31/3/94
½1994/95,by Craig Graham
=========================

Summary
-------
This program allows you to design finite state machines graphicaly, and
implement them in hardware using a p22v10 Programable Logic Array Chip.
This program comes as part of the CLA Digital Design System, although
it is not integrated in this version.

New in the release
-------------------
o Powerup State is now setable, for use with FSMsynth.
o Monochrome monitors are now supported as well as colour ones.
o ST-PASSM is now included.
o Supported output formats are now PASSM, PALASM2, and VHDL !!!!!
                                                       ^^^^
o It is recomended that FSMsynth is used for PAL generation as it is
  much more reliable that the FSMedit export options, and can support
  a wider range of devices.
  
THIS IS NOT A MANUAL, IT'S A DESCRIPTION.

Introduction
------------
Most programs in the CLA design suite have their drawbacks & faults. The
major one's with this program come from it being a rushed job. I needed
to design a finite state machine (actually, the Digitiser Control FSM
which is included as an example), so I wrote this program to help me do
it. I'd been intending to write one for a while, so it wasn't a real
hardship.


Limitations on this version
---------------------------
o Only p22v10's are supported.
o No output to CLA .NET format is supported. You have to export VHDL and then
  compile it using CLA-VHDL.
o Evaluated conditions are not finished.
o It is strongly recomended that you use FSMsynth to create PAL's with as it is
  much more reliable & fully featured than FSMedit's synthesis routines.

Design System
--------------
I've now included the ST version of PASSM with the package - note that this
is NOT part of CLA, it is a seperate PUBLIC DOMAIN package (source code is
included).

Although the manual is partly missing from the PASSM archive, I've worked
it out from the source code, and you'll never actually have to write any
PALASM code yourself as CLA-FSMedit does that bit for you. Just use PASSM
to compile the PAL files into a JEDEC fuse map.

Note however, that PASSM does have some bugs in it - you would be better to
use PALASM2 on a PC for the time being.

This is the design path I use at the moment :-

1) Atari Falcon running FSMedit to design & document the state machine.

      Export .PL2 file for PALASM2.

2) PC/emulator running PALASM2 for compiling & simulating.

      Export .JED file for device programming.

[17/5/94]
   My new path to simulation is via CLA-VHDL, which can compile a VHDL
   produced by FSMedit and produce a .NET file which can be loaded into
   the main CLA editor/simulator.

3) Universal Programmer for programming the device.

      PAL22V10 chip for testing.

4) Hewlett Packard Logic Analyser for testing the actual device.

File Formats
------------
This program generates 5 types of file.

 1) .FSM files are the CLA-FSMedit native file format, which it stores FSM's
    in.

    .PAL, .PL2, .VHD & .GEM files are exported via the TOOLBOX dialog window.

 2) .PAL files are output from CLA-FSMedit on demand and are PASSM source
    files, targetted for a p22v10. These can then be compiled using ST-PASSM
    to produce JEDEC fuse maps for programming a PAL. If you wish to aim for a
    different PAL, then REGISTER THE BLOODY PROGRAM, then  perhaps I'll help
    you. Watch out for the bugs in PASSM.

 3) .PL2 files contain the same information as .PAL files, but in PALASM2
    format. It must be said that using PALASM2 to compile designs is MUCH
    better than using PASSM, as you can target almost any device, and can
    run simulations & produce test vectors using the software - unfortunately
    it only runs on a PC. I will eventually get PALASM out of the loop by
    generating the JEDEC files direct, and allowing simulation of the PLD
    inside the main program, but for now I  have to recomend that you get
    PALASM2 and use a PC for final testing & PLD programming.

 4) .VHD files contain the same information as .PAL files, but describe the
    State Machine in VHSIC High Level Design Language (VHDL) - the modern
    standard language for ASIC & FPGA synthesis. You can then compile & simulate
    the FSM using the rest of the CLA package. The VHDL may also be used in
    'proper' design systems (eg. as part of a VHDL design, or as a block in
    a Intergraph design). So you can use your FSM designs in conjunction with other
    packages. Possibly the most significant addition so far ?

 5) .REP files are generated at the same time as .PAL & .PL2 files, and contain
    more detailled information about the FSM (eg, easily read pinouts, state bit
    assignments, transistion conditions, output variable states, etc).

 6) .GEM files are the standard GEM metafile format we all know & love.
    Works fine as long as you have GDOS installed with the metafile driver as
    device 31. Works with NVDI & Speedo. Note that the .GEM's are in colour.

Examples
--------
There are two example files included, B.FSM  &  V9.FSM.

B.FSM is the simple 3-state FSM which I used as a test piece for the program.
This will compile quite happily with PASSM on an ST, but isn't very complex.

V9.FSM is the state machine which I wrote the program to design in the first
place (it's the controller for a video digitiser/frame grabber).

You would require PALASM2 on a PC to compile the V9.FSM0 state machine due to the bug
in PASSM on the ST.

IMPORTANT NOTES:
---------------
As it's not immediately obvious, the  button at the far right of the editor
widget bar is the SELECT/EDIT icon - it allows you to select which state you
wish to edit, and also to drag that state to a new posistion on screen.

For the Output Metafile option to work, the GDOS META.SYS driver should be
installed as device 31.

FSMedit will accept a command line filename of an FSM - so you can use install
application from the desktop, using the document type .FSM
