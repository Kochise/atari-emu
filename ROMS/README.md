# TOS (The Operating System)

* Other sources

https://www.atariworld.org/tos-rom/

https://avtandil33.pythonanywhere.com/tose

https://mikro.naprvyraz.sk/docs/GEM/TOS.HTM

https://home.deds.nl/~dwvdburg/atari/framesets/frameset_tos.html

* Characteristics

| Version	| Date			| Machine				| Size		| Address	| GEMDOS	| VDI		| AES		| Note				|
| :---		| :---			| :---					| :---		| :---		| :---		| :---		| :---		| :---				|
| 1.00		| 1985/05/29	| ST					| 			| RAM		| 0.D0		| 			| 1.01		| Disk				|
| 1.00		| 1985/06/20	| ST					| 			| 			| 0.11		| 			| 1.01		| Mushroom TOS		|
| 1.00		| 1985/11/20	| ST					| 192 KB	| $FC0000	| 0.13		| 			| 1.20		| Alpha				|
| 1.00		| 1986/02/06	| ST					| 192 KB	| $FC0000	| 0.13		| 			| 1.20		| Beta				|
| 1.00		| 1986/04/24	| ST					| 192 KB	| $FC0000	| 0.13		| 			| 1.20		| 					|
| 1.02		| 1987/04/22	| STf - Mega ST			| 192 KB	| $FC0000	| 0.13		| 			| 1.20		| Blitter			|
| 1.04		| 1988/05/18	| STf - Mega ST	- STacy	| 192 KB	| $FC0000	| 0.15		| 			| 1.40		| Beta				|
| 1.04		| 1988/08/08	| STf - Mega ST	- STacy	| 192 KB	| $FC0000	| 0.15		| 			| 1.40		| Developer			|
| 1.04		| 1989/02/22	| STf - Mega ST	- STacy	| 192 KB	| $FC0000	| 0.15		| 			| 1.40		| 					|
| 1.04		| 1989/04/06	| STf - Mega ST	- STacy	| 192 KB	| $FC0000	| 0.15		| 			| 1.40		| Rainbow TOS		|
| 1.04 KaOS	| 1989/04/06	| ST					| 192 KB	| $FC0000	| 0.16		| 			| 1.41		| Custom TOS		|
| 1.43 KaOS	| 1989/04/06	| ST					| 192 KB	| $FC0000	| 0.16		| 			| 1.41		| Custom TOS fixed	|
| 1.06		| 1989/06/19	| STe					| 256 KB	| $E00000	| 0.15		| 			| 1.40		| 					|
| 1.06		| 1989/07/29	| STe					| 256 KB	| $E00000	| 0.15		| 			| 1.40		| Need STE_FIX.PRG	|
| 1.62		| 1990/01/01	| STe					| 256 KB	| $E00000	| 0.17		| 			| 1.40		| 1.06 fixed		|
| 2.02		| 1990			| STe - Mega STe		| 256 KB	| $E00000	| 			| 			| 			| 					|
| 2.05		| 1990/12/05	| STe - Mega STe		| 256 KB	| $E00000	| 0.19		| 			| 3.10		| 					|
| 2.06		| 1991/11/14	| STe - Mega STe (- ST)	| 256 KB	| $E00000	| 0.20		| 			| 3.20		| Fuji boot logo	|
| 2.07		| 1991			| Sparrow (aka "FX-1")	| 			| 			| 			| 			| 			| 					|
| 2.08		| 1992/03/10	| STBook				| 512 KB	| $E00000	| 			| 			| 			| ROM disk as P:	|
| 3.00		| 1989/05/23	| TT					| 512 KB	| $E00000	| 0.20		| 			| 3.20		| Beta				|
| 3.00		| 1990/03/01	| TT					| 512 KB	| $E00000	| 0.20		| 			| 3.20		| Developer			|
| 3.01		| 1990/08/09	| TT					| 512 KB	| $E00000	| 0.20		| 			| 3.20		| 					|
| 3.01		| 1990/08/29	| TT					| 512 KB	| $E00000	| 0.20		| 			| 3.20		| 					|
| 3.05		| 1990/12/05	| TT					| 512 KB	| $E00000	| 0.20		| 			| 3.20		| 					|
| 3.06		| 1991/09/24	| TT					| 512 KB	| $E00000	| 0.20		| 			| 3.20		| 					|
| 4.00		| 1992/08/11	| Falcon030				| 512 KB	| $E00000	| 0.30		| 			| 3.30		| Beta				|
| 4.01		| 1992/10/21	| Falcon030				| 512 KB	| $E00000	| 0.30		| 			| 3.40		| Developer			|
| 4.02		| 1993/01/26	| Falcon030				| 512 KB	| $E00000	| 0.30		| 			| 3.40		| 					|
| 4.03		| 1993			| Falcon030				| 512 KB	| $E00000	| 0.30		| 			| 3.40		| 					|
| 4.04		| 1993/03/08	| Falcon030				| 512 KB	| $E00000	| 0.30		| 			| 3.40		| 					|
| 4.92		| 1993/06/22	| Falcon030				| 512 KB	| RAM		| 0.30		| 			| 4.10		| Beta (MultiTOS)	|

This doesn't include the releases of : EmuTOS, MiNT (Not), MultiTOS (Now), FreeMINT, ...

```mermaid
%%{ init: { 'gitGraph': { 'mainBranchName': 'ST', 'rotateCommitLabel': true }, 'theme': 'base' }}%%
gitGraph TB:
	commit id: "1.00 Disk" tag: "850529"
	commit id: "1.00 Mush" tag: "850620"
	commit id: "1.00 alpha" tag: "851120"
	commit id: "1.00 beta" tag: "860206"
	commit id: "1.00" tag: "860424"
	commit id: "1.02" tag: "870422"
	commit id: "1.04 beta" tag: "880518"
	commit id: "1.04 dev" tag: "880808"
	commit id: "1.04" tag: "890406"
	branch KaOS
	checkout ST
	branch "Mega STe"
	checkout ST
	branch TT
	checkout ST
	checkout KaOS
	commit id: "1.04 KaOS" tag: "890406"
	commit id: "1.43 KaOS" tag: "890406"
	checkout ST
	checkout TT
	commit id: "3.00 beta" tag: "890523"
	checkout ST
	commit id: "1.06" tag: "890729"
	commit id: "1.62" tag: "900101"
	checkout TT
	commit id: "3.00 dev" tag: "900301"
	commit id: "3.01 dev" tag: "900809"
	commit id: "3.01" tag: "900829"
	checkout ST
	checkout "Mega STe"
	commit id: "2.05" tag: "901205"
	checkout TT
	commit id: "3.05" tag: "901205"
	commit id: "3.06" tag: "910924"
	checkout "Mega STe"
	commit id: "2.06" tag: "911114"
	commit id: "2.07 Sparrow" tag: "910101"
	checkout TT
	merge "Mega STe"
	branch Falcon030
	checkout "Mega STe"
	commit id: "2.08 STBook" tag: "920310"
	checkout Falcon030
	commit id: "4.00" tag: "920811"
	commit id: "4.01" tag: "921021"
	commit id: "4.02" tag: "930126"
	commit id: "4.04" tag: "930308"
	commit id: "4.92" tag: "930622"
```

## GEMDOS (GEM Disk Operating System)

| Version	| 0.11		| 0.13		| 0.16		| 0.17		| 0.19		| 0.20		| 0.30		|
| :---		| :---		| :---		| :---		| :---		| :---		| :---		| :---		|
| 			|

Alternative : PowerDOS

## VDI (Virtual Device Interface)

| Version	| 			| 			| 			| 			| 			| 			| 			|
| :---		| :---		| :---		| :---		| :---		| :---		| :---		| :---		|
| 			|

Alternative : QuickST, NVDI

## AES (Application Environment Services)

| Version	| 1.01		| 1.20		| 1.40		| 3.10		| 3.20		| 3.40		| 4.00		| 4.10		|
| :---		| :---		| :---		| :---		| :---		| :---		| :---		| :---		| :---		|
| 			|

Alternative : XaEAS, N.AES, MyAES

## GEM (Graphic Environment Manager)

| Version	| 			| 			| 			| 			| 			| 			| 			|
| :---		| :---		| :---		| :---		| :---		| :---		| :---		| :---		|
| 			|

Alternative : Gemini, Geneva, Magic

## GDOS (Graphics Device Operating System)

| Version	| 			| 			| 			| 			| 			| 			| 			|
| :---		| :---		| :---		| :---		| :---		| :---		| :---		| :---		|
| 			|

Alternative : AMC GDOS, G+Plus, FontGDOS, FSM GDOS, TTF-GDOS, SpeedoGDOS, NVDI
