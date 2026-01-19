# Utilities

Patches have been made by Atari and third parties to correct some TOS flaws.

| Version	| 1.00	| 1.02	| 1.04	| 1.06	| 1.62	| 2.05	| 2.06	| 3.00	| 3.01	| 3.05	| 3.06	| 4.00	| 4.01	| 4.02	| 4.04	| 4.92	|
| :---		| :---	| :---	| :---	| :---	| :---	| :---	| :---	| :---	| :---	| :---	| :---	| :---	| :---	| :---	| :---	| :---	|
| AUTOSORT	| X		| X		| X		| X		| X		| X		| X		| X		| X		| X		| X		| X		| X		| X		| X		| X		|
| FOLDRXXX	| X		| X		| X		| X		| X		| X		| X		| X		| X		| X		| X		| X		| X		| X		| X		| X		|
| CACHEXXX	| 		| 		| X		| X		| X		| X		| X		| X		| X		| X		| X		| X		| X		| X		| X		| X		|
| POOLFIX3	| 		| 		| X		| X		| 		| 		| 		| 		| 		| 		| 		| 		| 		| 		| 		| 		|
| TOS14FIX	| 		| 		| X		| 		| 		| 		| 		| 		| 		| 		| 		| 		| 		| 		| 		| 		|
| TOS14FX5	| 		| 		| X		| 		| 		| 		| 		| 		| X		| 		| 		| 		| 		| 		| 		| 		|
| STE_FIX	| 		| 		| 		| X		| 		| 		| 		| 		| 		| 		| 		| 		| 		| 		| 		| 		|
| HD_FDC	| 		| 		| 		| 		| 		| 		| X		| 		| 		| 		| 		| 		| 		| 		| 		| 		|
| SELTOS	| X		| 		| X		| 		| 		| 		| X		| 		| 		| 		| 		| 		| 		| 		| 		| 		|
| SERPTCH2	| 		| 		| 		| 		| 		| X		| 		| 		| 		| X		| 		| 		| 		| 		| 		| 		|
| COLORTOS	| 		| 		| 		| 		| 		| 		| X		| 		| 		| 		| X		| 		| 		| 		| 		| 		|
| BMAPFIX2	| 		| 		| 		| 		| 		| 		| 		| 		| 		| 		| 		| X		| X		| X		| X		| 		|
| TOS4_FIX	| 		| 		| 		| 		| 		| 		| 		| 		| 		| 		| 		| X		| X		| X		| X		| 		|
| FPATCH2	| 		| 		| 		| 		| 		| 		| 		| 		| 		| 		| 		| X		| X		| X		| X		| 		|
| FWRITE-3	| 		| 		| 		| 		| 		| 		| 		| 		| 		| 		| 		| X		| X		| X		| X		| 		|
| V_FILL	| 		| 		| 		| 		| 		| 		| 		| 		| 		| 		| 		| X		| X		| X		| X		| 		|

* AUTOSORT: sorts the programs in the AUTO folder
* FOLDRXXX: fixes 40 folders limit (unless using some HD driver that fixes it too)
* CACHEXXX: adds cache buffers
* POOLFIX3: fixes Gemdos pools
* TOS14FIX: fixes FOLDRXXX
* TOS14FX5: fixes TOS14FIX
* STE_FIX: fixes booting in Medium resolution
* HD_FDC: fixes TOS 2.06 drive seek to 3 ms
* SERPTCH2: fixes MFP and SCC serial ports
* COLORTOS: adds support for colour DESKICON.RSC
* BMAPFIX2: fixes Bconmap for TOS 4
* TOS4_FIX: fixes Gemdos regression for TOS 4
* FPATCH2: fixes bugs in TOS 4
* FWRITE-3: fixes something
* V_FILL: fixes V_ContourFill when color is OTHER_COLOR

* DSFIX: fixes Dosound with MultiTOS
* RAMTOS: allows to load a ROM image into RAM and reboot on it
* GEMRAM: copies the ROM to RAM to patch TOS directly (used by other patches)
* ARROWFIX: fixes the up/down arrow scroll click lock (included in WINX)
* SHBUF: fixes the NEWDESK.INF 4KB limit (after GEMRAM)
* WINX: adds desktop features (after GEMRAM)
* Y2K-FIX: fixes Y2000 bug
