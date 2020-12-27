CLA v3
========

This is the sixth beta release of the CLA3 electronic CAD system,
and the first general release. This release is just the schematic
and simulation sections (as far as I've got).

Anyone who has seen CLAv2 may be a little shocked by v3, as it is a
total re-write from scratch. Some features of v2 haven't made it into
v3, but the editor is a vast improvement over v2 (well, it should be,
I've spent a year or so on it). The simulator lacks some of the v2
features (interactive mode is the big one), but tries to make up for it
by being easier to use, and providing larger stimulus (the window now
scrolls).

If you've not seen v2 (and I hope everyone forgets about v1), you
should be in for a treat - CLAv3 aims to be a 'real' EDA system,
so it provides hierarchical designs, libraries, multiple windows, GDOS
printing, simulation, auto-routing, and lot's more, all wrapped up in
a DULIB GEM interface (windowed dialogs, AES4.1 support, colour icons,
etc).

Sorry, I've still not had time to fully update the Hypertext help, so
a lot of stuff is missing at the moment. I'm working on it.

Isn't it interesting to see that PC based EDA systems are starting to
look more like CLA ? I think they must have heard I was coming and
got worried :)

Show me a cheap PC program that can do what CLAv3 can do....
anyone who has used Electronics WorkBench (digital) will know
what I mean (the features are there mostly, but the way they are done
just sucks). Now - all I need is a PC version to make some money :(

NEW STUFF
==========
o The autorouter now knows about blocks.

DEVELOPMENT HELP
=================

I'd like a German transaltion of the docs if anyone is feeling adventurous.

I'm also looking for anyone who feels that they can contribute to
this system in a more active way (programming).

The source code is currently built using Lattice C 5.52, using the
DU_LIB GUI library (which was developed initially just for this task).
As DU_LIB is based mostly around callback functions for various things
rather than relying wholely on an event loop (although an event loop
can be used as well), anyone with X-Windows programming experience
would be a big help.

Area's that still need work:

o The FSMedit state machine editor is currently written in Basic, so a
   Basic->C conversion is needed, and then wrapping up as an accessory.

o The CLA-VHDL compiler needs a complete re-write to C.

o A postscript output driver for CLA has been a common request, and I don't
   know enough about postscript to do it myself.

o I've written a stand-alone synthesis tool FSMsynth, but it's just a command
   line driven compiler at the moment - needs a spot of GEM'ing up.

Get in touch if you want in.....

Craig.

  __                              _
  | \        | |                 / |
  |_/ A T A  \/| N C E R T A I N  /   - ECAD for the Atari/PC.
--------------------------------- o ----------------------------
Craig Graham. EMAIL:craig.graham@newcastle.ac.uk
                    craig.graham@adlib.centron.com

