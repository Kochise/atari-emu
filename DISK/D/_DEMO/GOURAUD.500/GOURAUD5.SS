; /// > Gouraud Cube - KOSS 2000/2001 < ///
;

; *** Equates Zone ************************************************************

; --- EQUATE SYSTEM ---

_ES_DAY:	EQU	31
_ES_MONTH:	EQU	10
_ES_YEAR:	EQU	2000
	; Date Format :	%AAAAAAAmmmmJJJJJ
_ES_DATE:	EQU	((_ES_YEAR-1980)<<9)|(_ES_MONTH<<5)|(_ES_DAY)

; --- EQUATE INDEX ---

; --- EQUATE DATA ---

GURO_ED_FILL:	EQU	0

	; *** Commands for Assembly

	OPT	p=68030/68882
	comment	HEAD=7	; FastLoad
	
;	output	\GOURAUD5.PRG

; /////////////////////////////////////////////////////////////////////////////
	SECTION	TEXT
; /////////////////////////////////////////////////////////////////////////////

	BRA	GURO_TU_PROG_Init

	; *** Program Informations

	dc.b	"ProgHEAD"	; ProgHEADER
	dc.w	$0100	; ProgVERSION
	dc.w	_ES_DATE	; ProgDATE
	dc.b	"Gouraud Cube",0	; ProgNAME
	dc.b	"Kochise",0	; ProgAUTHOR

; *** BasicPURPOSE ******************************************************
; *			*
; * 1. - Init the Memory		*
; * 2. + Run the Demo			*
; * 3. - Exit the Demo			*
; *			*
; ***************************************************** Set TAB to 24 ***

	; *** 1. Init the Memory ************************
	; *		*
	; * As ever, just keep the Program itself...	*
	; *		*
	; ***********************************************

GURO_TU_PROG_Init:	movea.l	(4,SP),A6	; Adresse de la BASEPAGE
	move.l	(12,A6),D0	; Taille du SEGMENT TEXT
	add.l	(20,A6),D0	; Additionne la taille du SEGMENT DATA … D0
	add.l	(28,A6),D0	; Additionne la taille du SEGMENT BSS … D0
	addi.l	#$100,D0	; Additionne la taille de la BASEPAGE … D0
	addi.l	#$400,D0	; Additionne la taille de la future PILE (Ici, 256 positions longues (1024 OCTETS))
	move.l	D0,D1	; Copie D0 dans D1 (Taille … r‚server)
	move.l	D0,D7
	add.l	A6,D1	; Additionne l'adresse du d‚but du programme avec D1 (La taille … r‚server)
	andi.l	#-2,D1	; Rend D1 pair (Au cas o— il ne l'‚tait pas)
	subq.l	#8,D1	; Garde de la place pour l'adresse de la BASEPAGE
	move.l	(4,SP),D2	; Sauvegarde l'adresse de la BASEPAGE
	movea.l	D1,SP	; Installe l'adresse de fin du programme+place nouvelle PILE dans la PILE
	move.l	D2,(4,SP)	; R‚installe l'adresse de la BASEPAGE
	move.l	D0,-(SP)	; EmPILE la taille … r‚server
	move.l	A6,-(SP)	; EmPILE l'adresse du programme
	clr.w	-(SP)	; R‚serv‚
	move.w	#$4A,-(SP)	; - M_SHRINK -
	trap	#1	; *** GEMDOS ***
	lea	(12,SP),SP

	; *** 2. Run the Demo ***************************
	; *		*
	; * 2.1. + Init the Demo	*
	; * 2.2. - Clear the Screens	*
	; * 2.3. + Precalculation of some Tables	*
	; * 2.4. + Main Loop of the demo	*
	; * 2.5. + Restore the States	*
	; *		*
	; ***********************************************

	; La D‚mo
	
	pea	GURO_SMTEXT_DEMO_Start
	move.w	#$26,-(SP)
	TRAP	#14	;Supexec
	addq.l	#6,SP

	; *** 3. Exit the Demo **************************
	; *		*
	; * Always NO Error...		*
	; *		*
	; ***********************************************

	; Sortie

	clr.w	-(SP)	; - P_TERM0 -
	trap	#1	; *** GEMDOS ***

	; ***********************************************

GURO_SMTEXT_DEMO_Start:	BSR	GURO_TS_INIT_Start
	BSR	GURO_TS_CLRS_Start
	BSR	GURO_TS_CALC_Start
	BSR	GURO_SMDEMO_LOOP_Start
	BSR	GURO_TS_EXIT_Start

	RTS
	
; -----------------------------------------------------------------------------

; *** 2. Run the Demo ***************************************************
; *			*
; * 2.1. + Init the Demo		*
; * 2.2. - Clear the Screens		*
; * 2.3. + Precalculation of some Tables		*
; * 2.4. + Main Loop of the Demo		*
; * 2.5. + Restore the Saved State		*
; *			*
; ***********************************************************************

; ----    ---------------------------------------------------------------------

	; *** 2.1. Init the Demo ************************
	; *		*
	; * 2.1.1. - Hide the Mouse	*
	; * 2.1.2. - Select the Monitor	*
	; * 2.1.3. - MAlloc two Screens	*
	; * 2.1.4. - Save Desktop Screen	*
	; * 2.1.5. - Save Desktop VideoMODE	*
	; * 2.1.6. - Wait the Next VBL	*
	; * 2.1.7. - Change both Resolution and Screens	*
	; * 2.1.8. - Set BorderColor to BLACK	*
	; *		*
	; ***********************************************

GURO_TS_INIT_Start:	; * 2.1.1. - Hide the Mouse *********************

	dc.w	$A00A	; LineA HideMOUSE

	; * 2.1.2. - Select the Monitor *****************

	move.w	#89,-(SP)	; - MON_TYPE -
	trap	#14	; *** XBIOS ***
	addq.l	#2,SP
		
	move.l	$4.w,GURO_TS_INIT_MonT
		; RESET Vector
	
	lea	GURO_TS_INIT_MonT,A0
	JMP	([A0,D0.w*4])
	
GURO_TS_INIT_MonT:	ds.l	1	; Monochrome
	dc.l	GURO_TS_INIT_RGB	; Couleur ST
	dc.l	GURO_TS_INIT_VGA	; VGA
	dc.l	GURO_TS_INIT_RGB	; TV

GURO_TS_INIT_RGB:	move.w	#$24,GURO_BDB_SCREEN_MODE
	move.l	#320*200*2,GURO_BDB_SCREEN_SIZE

	BRA	GURO_TS_INIT_Mem

GURO_TS_INIT_VGA:	move.w	#$134,GURO_BDB_SCREEN_MODE
	move.l	#320*240*2,GURO_BDB_SCREEN_SIZE

	; * 2.1.3. - MAlloc two Screens *****************

	; Adresses Ecrans

GURO_TS_INIT_Mem:	move.l	GURO_BDB_SCREEN_SIZE,D0
	mulu.l	#2,D0
	addq.l	#4,D0

	clr.w	-(SP)
	move.l	D0,-(SP)
	move.w	#$44,-(SP)	; - M_XALLOC -
	TRAP	#1	; *** GEMDOS ***
	addq.l	#8,SP

	move.l	D0,GURO_BDB_SCREEN_MEMORY

	addq.l	#4,D0
	andi.l	#-4,D0

	move.l	D0,GURO_BDB_SCREEN_FLIP_0
	add.l	GURO_BDB_SCREEN_SIZE,D0
	move.l	D0,GURO_BDB_SCREEN_FLIP_1
	
	; * 2.1.4. - Save DeskTOP Screen ****************

	; Ecran Physique du GEM

	move.w	#2,-(SP)	; - PHYSBASE -
	TRAP	#14	; *** XBIOS ***
	addq.w	#2,SP

	move.l	D0,GURO_BDB_DESKT_SCREEN

	; * 2.1.5. - Save DeskTOP VideoMODE *************

	; Sauve le Mode Vid‚o

	move.w	#-$1,-(SP)
	move.w	#$58,-(SP)	; - VSETMODE -
	TRAP	#14	; *** XBIOS ***
	addq.l	#4,SP

	move.w	D0,GURO_BDB_DESKT_MODE

	; * 2.1.6. - Wait the Next VBL ******************

	; On attend

	move.w	#$25,-(SP)	; - VSYNC -
	TRAP	#14	; *** XBIOS ***
	addq.l	#2,SP

	; * 2.1.7. - Change both Resolution and Screens *

	; Nouveaux Ecrans

	move.w	GURO_BDB_SCREEN_MODE,-(SP)
	move.w	#3,-(SP)
	move.l	GURO_BDB_SCREEN_FLIP_0,-(SP)
	move.l	GURO_BDB_SCREEN_FLIP_0,-(SP)
	move.w	#5,-(SP)	; - VSETSCREEN -
	TRAP	#14	; *** XBIOS ***
	lea	(14,SP),SP

	; * 2.1.8. - Set BorderCOLOR to BLACK ***********

	; Couleur de Fond NOIRE

	clr.l	$FFFF9800	; FalcPal0

	RTS

; -----------------------------------------------------------------------------

	; *** 2.2. Clear the Screens ********************
	; *		*
	; * Nothing realy difficult there. I will JUST  *
	; * Fill the both Screens previously allocated  *
	; * with an Unique Color.	*
	; *		*
	; ***********************************************


GURO_TS_CLRS_Start:	; Efface les Ecrans

	movem.l	GURO_BDB_SCREEN_FLIP_0,A0-A1
	move.l	GURO_BDB_SCREEN_SIZE,D0
	asr.l	#2,D0
	move.w	#GURO_ED_FILL,D1
	swap	D1
	move.w	#GURO_ED_FILL,D1	

	; 16 LONGs : 32 BYTEs

GURO_TS_CLRS_Loop:	move.l	D1,(A0)+
	move.l	D1,(A1)+

	subq.l	#1,D0
	BNE	GURO_TS_CLRS_Loop

	RTS

; -----------------------------------------------------------------------------

	; *** 2.3. Precalculation of some Tables ********
	; *		*
	; * Instead of RealTIME Calculation, because    *
	; * a 68030 @ 16 MHz is - SADLY - hardly enough *
	; * for the Huge work necessary for the 3D Rout *
	; * and the GouraudSHADING Algorythm, I'll use  *
	; * some Tricky PrecaluclatedTABLES.            *
	; *		*
	; * 2.3.1. - Complete 1.5 Loop Sin/CosTABLE	*
	; * 2.3.2. - Linear *6 TABLE (of 200 lines)	*
	; * 2.3.3. - GURO_BDB_L0029_TABLE	*
	; * 2.3.4. - Linear *640 TABLE (of 200 lines)	*
	; *		*
	; ***********************************************

GURO_TS_CALC_Start:	; * 2.3.1. - Complete 1.5 Loop Sin/CosTABLE *****

	lea	GURO_DDA_COSIN_BANK,A0
	lea	GURO_BDB_COSIN_TABLE,A1

	movem.w	(A0)+,D0-D2

	fmove.w	D1,FP0
	fmove.w	D2,FP1
	fdiv.x	FP1,FP0
	fmove.w	#180,FP1
	fdiv.x	FP1,FP0
	fmovecr.x	#0,FP1	; PI
	fmul.x	FP1,FP0

	fmovecr.x	#15,FP1	; 0
	fmove.w	#256,FP2

GURO_TS_CALC_SinT:	fmove.x	FP1,FP3

	fsin.x	FP3

	fmul.x	FP2,FP3	; 256

	fmove.w	FP3,(A1)+

	fadd.x	FP0,FP1

	DBF	D0,GURO_TS_CALC_SinT

	; * 2.3.2. - Linear *6 TABLE ********************

	lea	GURO_BDB_LIN6_TABLE,A0

	move.w	#199,D0	; 200 Lines

GURO_TS_CALC_Lin6:	move.w	D0,D1
	mulu.w	#6,D1

	move.w	D1,(A0,D0.w*2)

	DBF	D0,GURO_TS_CALC_Lin6

	; * 2.3.3. - GURO_BDB_L0029_TABLE ******************************

	lea	GURO_DDA_L0029_BANK,A0
	lea	GURO_BDB_L0029_TABLE,A1

	movem.w	(A0)+,D0-D1

GURO_TS_CALC_L0029Init:
	move.w	D1,D2
	movem.w	(A0)+,D3-D5

GURO_TS_CALC_L0029Tab:
	move.w	D3,(A1)+
	add.w	D4,D3
	move.w	D3,(A1)+
	add.w	D4,D3

	add.w	D5,D3

	DBF	D2,GURO_TS_CALC_L0029Tab
	
	DBF	D0,GURO_TS_CALC_L0029Init

	; * 2.3.4. - Linear *640 TABLE ******************

	lea	GURO_BDB_LIN640_TABLE,A0

	move.l	#199,D0	; 200 Lines
	move.l	#640,D1
	clr.l	D2

GURO_TS_CALC_Lin640:
	move.l	D2,(A0)+
	add.l	D1,D2

	DBF	D0,GURO_TS_CALC_Lin640

	RTS

; -----------------------------------------------------------------------------

	; *** 2.4. Main Loop of the Demo ****************
	; *		*
	; * 2.4.1. + Swap Screens	*
	; * 2.4.2. - Invert X Table	*
	; * 2.4.3. + Calc Shape's 3D Coordinates	*
	; * 2.4.4. + Fill Shape's Faces	*
	; * 2.4.5. + Mask Shape's Previous Faces	*
	; * 2.4.6. - Change View Coordinates	*
	; * 2.4.7. - Check the SpaceBAR	*
	; *		*
	; ***********************************************

GURO_SMDEMO_LOOP_Start:	; * 2.4.1. + Swap Screens ***********************

	BSR	GURO_TS_SWAP_Start

	; * 2.4.2. - Invert X Table *********************

	lea	GURO_DAT_L0023,A0
	movem.l	(A0),D0-D3
	move.l	D3,(A0)+
	move.l	D2,(A0)+
	move.l	D0,(A0)+
	move.l	D1,(A0)+

	; * 2.4.3. + Calc Shape's 3D Coordinates ********

	BSR	GURO_TS_PLOT_Start

	; * 2.4.4. + Fill Shape's Faces *****************

	BSR	GURO_TS_FILL_Start

	; * 2.4.5. + Mask Shape's Previous Faces ********

	BSR	GURO_TS_MASK_Start

	; * 2.4.6. - Change View Coordinates ************

	addq.b	#3,GURO_DDA_AXIS_X+1
	addq.b	#2,GURO_DDA_AXIS_Y+1
	addq.b	#1,GURO_DDA_AXIS_Z+1

	; * 2.4.7. - Check the SpaceBAR *****************

	cmpi.b	#$39,$FFFFFC02.w	; ACIA - IKBD
	BNE	GURO_SMDEMO_LOOP_Start

	RTS

; ----    ---------------------------------------------------------------------

	; *** 2.4.1. Swap Screens ***********************
	; *		*
	; * 2.4.1.1. - Wait the Next VBL	*
	; * 2.4.1.2. - Flip Screens Addresses	*
	; * 2.4.1.3. - Set the New Screen	*
	; *		*
	; ***********************************************

GURO_TS_SWAP_Start:	; * 2.4.1.1. - Wait the Next VBL ****************

	move.w	#$25,-(SP)	; - VSYNC -
	TRAP	#14	; *** XBIOS ***
	addq.w	#2,SP

	; * 2.4.1.2. - Flip Screens Addresses ***********
	
	movem.l	GURO_BDB_SCREEN_FLIP_0,D0-D1
	exg	D1,D0
	movem.l	D0-D1,GURO_BDB_SCREEN_FLIP_0

	; * 2.4.1.3. - Set the New Screen ***************
	
	move.w	#-1,-(SP)
	move.l	D1,-(SP)
	move.l	D1,-(SP)
	move.w	#5,-(SP)	; - SETSCREEN -
	TRAP	#14	; *** XBIOS ***
	lea	(12,SP),SP

	RTS

; -----------------------------------------------------------------------------

	; *** 2.5. Restore the Saved State **************
	; *		*
	; * 2.5.1. - Restore both Resolution and Screen *
	; * 2.5.2. - Set BorderCOLOR to WHITE	*
	; * 2.5.3. - Free the Screens of the Demo	*
	; *		*
	; ***********************************************

GURO_TS_EXIT_Start:	; * 2.5.1. - Restore both Resolution and Screen *
	
	move.w	GURO_BDB_DESKT_MODE,-(SP)
	move.w	#3,-(SP)
	move.l	GURO_BDB_DESKT_SCREEN,-(SP)
	move.l	GURO_BDB_DESKT_SCREEN,-(SP)
	move.w	#5,-(SP)	; - VSETSCREEN -
	TRAP	#14	; *** XBIOS ***
	lea	(14,SP),SP

	; * 2.5.2. - Set BorderCOLOR to WHITE ***********

	move.l	#-1,$FFFF9800	; FalcPal0

	; * 2.5.3. - Free the Screens of the Demo *******

	; Lib‚ration des Ecrans

	move.l	GURO_BDB_SCREEN_MEMORY,-(SP)
	move.w	#$49,-(SP)	; - M_FREE -
	TRAP	#1	; *** GEMDOS ***
	addq.l	#6,SP

	RTS

; -----------------------------------------------------------------------------
; *** The 3D Routines *********************************************************
; -----------------------------------------------------------------------------

; *** 3D Routines *******************************************************
; *			*
; *			*
; ***********************************************************************

; ----    ---------------------------------------------------------------------

GURO_TS_PLOT_Start:	lea	GURO_DDA_AXIS_BANK,A0
	lea	GURO_BDB_CORNER_TABLE,A1

	lea	GURO_BDB_COSIN_TABLE,A2	; sin 000/090
	lea	(64*2,A2),A6	; cos 000/090

	lea	(6,A0),A4	; rX-rY-rZ
	lea	(12,A0),A3	; pDATA

	move.w	(A3)+,D6	; NbPoint ->ObjectDATA
	move.w	D6,D7
	subq.w	#1,D7	; Loopa

	; NbPoint*6
	move.w	(GURO_BDB_LIN6_TABLE.l,D6.w*2),D6
	lea	(A3,D6.w),A5	; ->ObjectSHADE

; --- Loopa ---

GURO_TS_PLOT_Light:	; ,---- LIGHT INTENSITY Calculation -------------------
	; | We'll calculate first the color intensity of the
	; | edge corner of the cube.

	move.w	(A5)+,(A1)	; sX
	movem.w	(A5)+,D0-D1	; sY,sZ
	movem.w	D0-D1,(2,A1)

	move.w	(A4),D6	; rX
	
	BSR	GURO_TS_PLOT_Coord
	
	movem.w	D0-D1,(2,A1)

	; ---

	move.w	(A1),D0	; sX

	move.w	(2,A4),D6	; rY
	move.w	(A2,D6.w*2),D4	; sin rY
	move.w	(A6,D6.w*2),D5	; cos rY

	move.w	D0,D2	; sX

	muls.w	D5,D1	; [(sZ cos rX)+(sY sin rX)]/cos rY
	muls.w	D4,D2	; sX sin rY

	add.l	D2,D1	; [((sZ cos rX)+(sY sin rX))/cos rY]+(sX sin rY)
	asr.l	#8,D1

	neg.w	D1
	BPL	GURO_TS_PLOT_Clip

	clr.w	D1	; If NEGATIVE Intensity 

GURO_TS_PLOT_Clip:	move.w	D1,(A1)+

	; ,---- 3D OBJECT Projection --------------------------
	; | Now, we'll calculate the 3D Projection of the
	; | edge corner of the cube.

	move.w	(A3)+,(A1)	; cX
	movem.w	(A3)+,D0-D1	; cY,cZ
	movem.w	D0-D1,(2,A1)
	
	move.w	(A4),D6	; rX
	
	BSR	GURO_TS_PLOT_Coord
	
	movem.w	D0-D1,(2,A1)

	; ---

	move.w	(A1),D0

	move.w	(2,A4),D6	; rY
	
	BSR	GURO_TS_PLOT_Coord
	
	move.w	D0,(A1)
	move.w	D1,(4,A1)
	
	; ---
	
	move.w	(2,A1),D1
	
	move.w	(4,A4),D6	; rZ
	
	BSR	GURO_TS_PLOT_Coord

	; ---

	move.w	(4,A1),D2

	add.w	(A0),D0	; aX
	add.w	(2,A0),D1	; aY
	add.w	(4,A0),D2	; aZ

	ext.l	D0
	ext.l	D1
	ext.l	D2

	asl.l	#8,D0
	asl.l	#8,D1

	divsl.l	D2,D0:D0
	divsl.l	D2,D1:D1

	addi.w	#160,D0	; CoordX
	addi.w	#100,D1	; CoordY

	movem.w	D0-D1,(A1)
	addq.w	#4,A1

	; Loopa
	DBF	D7,GURO_TS_PLOT_Light

	RTS

; ---

GURO_TS_PLOT_Coord:	move.w	(A2,D6.w*2),D4	; sin r.
	move.w	(A6,D6.w*2),D5	; cos r.
	
	move.w	D0,D2	; x1
	move.w	D1,D3	; x2

	muls.w	D5,D0	; x1 cos r.
	muls.w	D5,D1	; x2 cos r.
	muls.w	D4,D2	; x1 sin r.
	muls.w	D4,D3	; x2 sin r.

	sub.l	D3,D0	; (x1 cos r.)-(x2 sin r.)
	add.l	D2,D1	; (x2 cos r.)+(x1 sin r.)

	asr.l	#8,D0	; [(x1 cos r.)-(x2 sin r.)]/256
	asr.l	#8,D1	; [(x2 cos r.)+(x1 sin r.)]/256

	RTS

; -----------------------------------------------------------------------------

GURO_TS_FILL_Start:	lea	GURO_DDA_AXIS_BANK,A0
	lea	GURO_BDB_CORNER_TABLE,A3

	lea	(12,A0),A0	; pDATA

	move.w	(A0)+,D6	; NbPoint ->ObjectDATA
	move.w	D6,D7

	mulu.w	#6,D6
	mulu.w	#12,D7
	
	lea	(A0,D7.w),A2	; ObjectFACE
	lea	(A0,D6.w),A4	; ObjectSHADE

	move.w	#1,GURO_BDB_L0006
	move.w	(A2)+,GURO_BDB_L0037
	
	; ---
	
	BSR	GURO_LoadLineIndex
	
	movea.l	GURO_DAT_L0023,A0

	move.w	D1,(A0)+
	move.w	D1,(A0)+

	BRA	L0008

T_432:	BSR	GURO_LoadLineIndex

L0008:	move.w	(A2)+,D2
	move.w	(GURO_BDB_LIN6_TABLE.l,D2.w*2),D2

	move.w	(A3,D2.w),D5
	BPL	T_460

	moveq.l	#0,D5

T_460:	movem.w	(2,A3,D2.w),D2/D3

	cmp.w	D3,D6
	BLT	T_46C

	move.w	D3,D6

T_46C:	cmp.w	D3,D7
	BGT	T_472

	move.w	D3,D7

T_472:	movem.w	D2/D3/D5,-(SP)

	BSR	T_4B6

	movem.w	(SP)+,D0-D1/D4

	subq.w	#1,GURO_BDB_L0038
	BPL	L0008

	movem.w	D6/D7,GURO_BDB_L000F
	move.w	(A2)+,D0

	sub.w	D6,D7
	BEQ	T_4A8

	subq.w	#1,D7
	lsl.w	#7,D0
	lea	(GURO_BDB_L0029_TABLE.l,D0.w),A0
	
	movea.l	GURO_BDB_SCREEN_FLIP_0,A6

	BSR	T_508

T_4A8:	subq.w	#1,GURO_BDB_L0037
	BPL	T_432

T_4B4:	RTS

; ---

GURO_LoadLineIndex:	move.w	(A2)+,GURO_BDB_L0038
	
	move.w	(A2),D0

	lsl.w	#1,D0
	add.w	(A2)+,D0
	lsl.w	#1,D0

	move.w	(A3,D0.w),D4
	movem.w	(2,A3,D0.w),D0-D1

	move.w	D1,D6
	move.w	D1,D7

	RTS

; ---

T_4B6:	lea	GURO_BDB_L0031,A0
	lea	GURO_BDB_L0032,A1

	cmp.w	D1,D3
	BGT	T_4D2
	BEQ	T_4B4

	exg	D0,D2
	exg	D1,D3
	exg	D4,D5

	addq.w	#2,A0
	addq.w	#2,A1

T_4D2:	sub.w	D0,D2
	sub.w	D1,D3
	sub.w	D4,D5

	lsl.w	#2,D1

	adda.w	D1,A0
	adda.w	D1,A1

	ext.l	D3

	swap	D2
	divsl.l	D3,D2:D2

	swap	D5
	divsl.l	D3,D5:D5

L0009:	move.w	D0,(A0)+

	swap	D0
	add.l	D2,D0

	swap	D0
	addq.w	#2,A0

	move.w	D4,(A1)+

	swap	D4
	add.l	D5,D4

	swap	D4
	addq.w	#2,A1

	DBF	D3,L0009

T_506:	RTS

; ---

T_508:	move.l	(GURO_BDB_LIN640_TABLE.l,D6.w*4),D0
	adda.l	D0,A6
	lea	(GURO_BDB_L0031.l,D6.w*4),A1

L000A:	movem.w	(800,A1),D2/D3
	movem.w	(A1)+,D0-D1
	lea	(A6,D0.w*2),A5

	sub.w	D0,D1
	BEQ	T_5E0
	BMI	T_506

	ext.l	D1
	sub.w	D2,D3
	swap	D3
	clr.w	D3
	divsl.l	D1,D3:D3
	move.w	D1,D0
	lsr.w	#5,D1
	andi.w	#$1F,D0
	lsl.w	#2,D0
	neg.w	D0
	swap	D2
	move.w	#$8000,D2

	add.w	D3,D2
	swap	D2
	swap	D3

	JMP	(L000C.l,D0.w)

L000B:	move.w	D2,(A5)+
	addx.l	D3,D2

	move.w	D2,(A5)+
	addx.l	D3,D2

	move.w	D2,(A5)+
	addx.l	D3,D2

	move.w	D2,(A5)+
	addx.l	D3,D2

	move.w	D2,(A5)+
	addx.l	D3,D2

	move.w	D2,(A5)+
	addx.l	D3,D2

	move.w	D2,(A5)+
	addx.l	D3,D2

	move.w	D2,(A5)+
	addx.l	D3,D2

	move.w	D2,(A5)+
	addx.l	D3,D2

	move.w	D2,(A5)+
	addx.l	D3,D2

	move.w	D2,(A5)+
	addx.l	D3,D2

	move.w	D2,(A5)+
	addx.l	D3,D2

	move.w	D2,(A5)+
	addx.l	D3,D2

	move.w	D2,(A5)+
	addx.l	D3,D2

	move.w	D2,(A5)+
	addx.l	D3,D2

	move.w	D2,(A5)+
	addx.l	D3,D2

	move.w	D2,(A5)+
	addx.l	D3,D2

	move.w	D2,(A5)+
	addx.l	D3,D2

	move.w	D2,(A5)+
	addx.l	D3,D2

	move.w	D2,(A5)+
	addx.l	D3,D2

	move.w	D2,(A5)+
	addx.l	D3,D2

	move.w	D2,(A5)+
	addx.l	D3,D2

	move.w	D2,(A5)+
	addx.l	D3,D2

	move.w	D2,(A5)+
	addx.l	D3,D2

	move.w	D2,(A5)+
	addx.l	D3,D2

	move.w	D2,(A5)+
	addx.l	D3,D2

	move.w	D2,(A5)+
	addx.l	D3,D2

	move.w	D2,(A5)+
	addx.l	D3,D2

	move.w	D2,(A5)+
	addx.l	D3,D2

	move.w	D2,(A5)+
	addx.l	D3,D2

	move.w	D2,(A5)+
	addx.l	D3,D2

	move.w	D2,(A5)+
	addx.l	D3,D2

L000C:	DBF	D1,L000B

T_5E0:	lea	(640,A6),A6	; Next Line

	DBF	D7,L000A

	movem.w	GURO_BDB_L000F,D6/D7
	movea.l	GURO_DAT_L0023,A0
	lea	GURO_BDB_L0031,A6
	movem.w	(A0)+,D4/D5

	BSR	L0013

	movem.w	GURO_BDB_L000F,D6/D7
	movea.l	GURO_DAT_L0023,A0
	movem.w	(A0),D4/D5

	cmp.w	D6,D4
	BLE	L000D

	move.w	D6,D4

L000D:	cmp.w	D5,D7
	BLE	L000E

	move.w	D7,D5

L000E:	movem.w	D4/D5,(A0)

	RTS

; ---

L0010:	clr.w	GURO_BDB_L0006

	movem.w	D6-D7,(-4,A0)
	lea	(A0,D6.w*4),A0
	lea	(A6,D6.w*4),A6
	sub.w	D6,D7

L0011:	move.l	(A6)+,(A0)+

	DBRA	D7,L0011

L0012:	RTS

; ---

L0013:	cmp.w	D6,D7
	BLE	L0012

	tst.w	GURO_BDB_L0006
	BNE	L0010

	cmp.w	D4,D6
	BGE	L0014

	cmp.w	D7,D4
	BGE	L0015

	cmp.w	D5,D7
	BGE	L001B

	BRA	L001C

L0014:	cmp.w	D5,D6
	BGE	L0015

	cmp.w	D5,D7
	BGE	L0016

	BRA	L0017

L0015:	lea	(A0,D6.w*4),A0
	lea	(A6,D6.w*4),A6
	
	sub.w	D6,D7
	subq.w	#1,D7
	BLT	T_68A

T_684:	move.l	(A6)+,(A0)+

	DBF	D7,T_684

T_68A:	RTS

; ---

L0016:	lea	(A0,D6.w*4),A0
	lea	(A6,D6.w*4),A6

	move.w	D5,D4
	
	sub.w	D6,D5
	subq.w	#1,D5
	BLT	T_6B4

T_69C:	move.w	(A6)+,D1

	cmp.w	(A0),D1
	BLE	T_6A4

	move.w	(A0),D1

T_6A4:	move.w	D1,(A0)+
	move.w	(A6)+,D1

	cmp.w	(A0),D1
	BGE	T_6AE

	move.w	(A0),D1

T_6AE:	move.w	D1,(A0)+

	DBF	D5,T_69C

T_6B4:	sub.w	D4,D7

	subq.w	#1,D7
	BLT	T_6C0

T_6BA:	move.l	(A6)+,(A0)+

	DBF	D7,T_6BA

T_6C0:	RTS

; ---

L0017:	lea	(A0,D6.w*4),A0
	lea	(A6,D6.w*4),A6
	sub.w	D6,D7

	subq.w	#1,D7
	BLT	T_6E8

L0018:	move.w	(A6)+,D1

	cmp.w	(A0),D1
	BLE	L0019

	move.w	(A0),D1

L0019:	move.w	D1,(A0)+
	move.w	(A6)+,D1

	cmp.w	(A0),D1
	BGE	L001A

	move.w	(A0),D1

L001A:	move.w	D1,(A0)+

	DBF	D7,L0018

T_6E8:	RTS

; ---

L001B:	lea	(A0,D6.w*4),A0
	lea	(A6,D6.w*4),A6

	move.w	D4,D0
	sub.w	D6,D0

	subq.w	#1,D0
	BLT	T_702

T_6FA:	move.l	(A6)+,(A0)+

	DBF	D0,T_6FA

	sub.w	D5,D7

T_702:	sub.w	D4,D5

	subq.w	#1,D5
	BLT	T_720

T_708:	move.w	(A6)+,D1

	cmp.w	(A0),D1
	BLE	T_710

	move.w	(A0),D1

T_710:	move.w	D1,(A0)+
	move.w	(A6)+,D1

	cmp.w	(A0),D1
	BGE	T_71A

	move.w	(A0),D1

T_71A:	move.w	D1,(A0)+

	DBF	D5,T_708

T_720:	subq.w	#1,D7
	BLT	T_72A

T_724:	move.l	(A6)+,(A0)+

	DBF	D7,T_724

T_72A:	RTS

; ---

L001C:	lea	(A0,D6.w*4),A0
	lea	(A6,D6.w*4),A6

	move.w	D4,D0
	sub.w	D6,D0

	subq.w	#1,D0
	BLT	L001E

L001D:	move.l	(A6)+,(A0)+

	DBF	D0,L001D

L001E:	sub.w	D4,D7

	subq.w	#1,D7
	BLT	L0022

L001F:	move.w	(A6)+,D1

	cmp.w	(A0),D1
	BLE	L0020

	move.w	(A0),D1

L0020:	move.w	D1,(A0)+
	move.w	(A6)+,D1

	cmp.w	(A0),D1
	BGE	L0021

	move.w	(A0),D1

L0021:	move.w	D1,(A0)+

	DBF	D7,L001F

L0022:	RTS

; -----------------------------------------------------------------------------

GURO_TS_MASK_Start:	movea.l	GURO_DAT_L0024,A0
	movea.l	GURO_DAT_L0023,A1
	movem.w	(A0)+,D4/D5

	tst.w	D4
	BEQ	GURO_ClearEND

	movem.w	(A1)+,D6/D7

	tst.w	D6
	BEQ	GURO_ClearEND

	cmp.w	D4,D6
	BLE	T_30C

	move.w	D6,D1
	lea	(A0,D4.w*4),A2
	sub.w	D4,D1
	move.l	(GURO_BDB_LIN640_TABLE.l,D4.w*4),D4
	movea.l	GURO_BDB_SCREEN_FLIP_0,A5
	adda.l	D4,A5

T_2F0:	move.w	(A2)+,D4
	lea	(A5,D4.w*2),A4

	sub.w	(A2)+,D4
	BGE	T_304

	addq.w	#1,D4
	neg.w	D4

T_2FE:	move.w	#GURO_ED_FILL,(A4)+

	DBF	D4,T_2FE

T_304:	lea	(640,A5),A5

	DBF	D1,T_2F0

T_30C:	cmp.w	D5,D7
	BLE	T_35C

	lea	(A0,D6.w*4),A2
	lea	(A1,D6.w*4),A3
	sub.w	D6,D5
	move.l	(GURO_BDB_LIN640_TABLE.l,D6.w*4),D6
	movea.l	GURO_BDB_SCREEN_FLIP_0,A5
	adda.l	D6,A5

L0004:	move.w	(A2)+,D0
	move.w	(A3)+,D1
	sub.w	D0,D1

	subq.w	#1,D1
	BLT	T_33E

	lea	(A5,D0.w*2),A4

T_338:	move.w	#GURO_ED_FILL,(A4)+

	DBF	D1,T_338

T_33E:	move.w	(A2)+,D1
	move.w	(A3)+,D0
	sub.w	D0,D1

	subq.w	#1,D1
	BLT	T_352

	lea	(A5,D0.w*2),A4

L0005:	move.w	#GURO_ED_FILL,(A4)+

	DBF	D1,L0005

T_352:	lea	(640,A5),A5

	DBF	D5,L0004

GURO_ClearEND:	RTS

; ---

T_35C:	move.w	D6,D2
	move.w	D7,D3
	lea	(A0,D6.w*4),A2
	lea	(A1,D6.w*4),A3
	sub.w	D2,D3
	move.l	(GURO_BDB_LIN640_TABLE.l,D2.w*4),D2
	movea.l	GURO_BDB_SCREEN_FLIP_0,A5
	adda.l	D2,A5

T_37A:	move.w	(A2)+,D0
	move.w	(A3)+,D1
	sub.w	D0,D1

	subq.w	#1,D1
	BLT	T_38E

	lea	(A5,D0.w*2),A4

T_388:	move.w	#GURO_ED_FILL,(A4)+

	DBF	D1,T_388

T_38E:	move.w	(A2)+,D1
	move.w	(A3)+,D0
	sub.w	D0,D1

	subq.w	#1,D1
	BLT	T_3A2

	lea	(A5,D0.w*2),A4

T_39C:	move.w	#GURO_ED_FILL,(A4)+

	DBF	D1,T_39C

T_3A2:	lea	(640,A5),A5

	DBF	D3,T_37A

	lea	(A0,D7.w*4),A2
	sub.w	D7,D5
	move.l	(GURO_BDB_LIN640_TABLE.l,D7.w*4),D7
	movea.l	GURO_BDB_SCREEN_FLIP_0,A5
	adda.l	D7,A5

T_3C2:	move.w	(A2)+,D7
	lea	(A5,D7.w*2),A4

	sub.w	(A2)+,D7
	BGE	T_3D6

	addq.w	#1,D7
	neg.w	D7

T_3D0:	move.w	#GURO_ED_FILL,(A4)+

	DBF	D7,T_3D0

T_3D6:	lea	(640,A5),A5

	DBF	D5,T_3C2

	RTS


; /////////////////////////////////////////////////////////////////////////////
	SECTION	DATA
; /////////////////////////////////////////////////////////////////////////////

; --- DATA ADDRESS TABLE ---

GURO_DAT_L0023:	dc.l	GURO_BDB_L0033

GURO_DAT_L0024:	dc.l	GURO_BDB_L0034
	dc.l	GURO_BDB_L0035
	dc.l	GURO_BDB_L0036

; --- DATA DEFINED ARRAY ---

GURO_DDA_COSIN_BANK:	dc.w	383,90,64	

GURO_DDA_L0029_BANK:	dc.w	6,31
	dc.w	0,0,2048
	dc.w	0,0,2049
	dc.w	0,32,0
	dc.w	0,32,2048
	dc.w	0,0,1
	dc.w	31,32,2048
	dc.w	0,32,2049

GURO_DDA_AXIS_BANK:	dc.w	0,0,1402	; aX,aY,aZ

GURO_DDA_AXIS_X:	dc.w	$20	; rX
GURO_DDA_AXIS_Y:	dc.w	$20	; rY
GURO_DDA_AXIS_Z:	dc.w	0	; rZ

	; +12

GURO_DDA_OBJECT_DATA:	dc.w	8	; NbPoint

	; ObjectDATA
	;             Y                             Z
	;             ^                             ^
	;      FRONT  |   VIEW               TOP    |   VIEW
	;    3 ,_____________, 2           7 ,_____________, 6
	;      |\           /|               |\           /|
	;     7| \____|____/ |6             4| \____|____/ |5
	;      | |    +    | |               | |    +    | |
	;      | |    |z   | |               | |    |Y   | |
	;  ----| +----+----+ |----> X    ----| +----O----+ |----> X
	;      | |    |    | |               | |    |    | |
	;     4| |____+____| |5             0| |____+____| |1
	;      | /    |    \ |               | /    |    \ |
	;      |/           \|               |/           \|
	;    0 'ÿÿÿÿÿÿÿÿÿÿÿÿÿ' 1           3 'ÿÿÿÿÿÿÿÿÿÿÿÿÿ' 2
	;             |                             |

	dc.w	-200,-200,-200	; cX,cY,cZ
	dc.w	200,-200,-200
	dc.w	200,200,-200
	dc.w	-200,200,-200
	dc.w	-200,-200,200
	dc.w	200,-200,200
	dc.w	200,200,200
	dc.w	-200,200,200

	; ObjectSHADE

	dc.w	-16,-16,-16	; sX,sY,sZ
	dc.w	16,-16,-16
	dc.w	16,16,-16
	dc.w	-16,16,-16
	dc.w	-16,-16,16
	dc.w	16,-16,16
	dc.w	16,16,16
	dc.w	-16,16,16

	; ObjectFACE
	;             Y                             Z
	;             ^                             ^
	;      FRONT  |   VIEW               TOP    |   VIEW
	;      ,_____________,               ,_____________,  
	;      |\       3   /|               |\       5   /|
	;      |4\____|____/ |               | \____|____/ | 
	;      | |    +5   | |               | |    +1   | |
	;      | |    |z   | |               |4|    |Y   | |
	;  ----| +----+----+ |----> X    ----| +----O----+ |----> X
	;      | |    |    | |               | |    |    |2|
	;      | |____+____| |               | |____+____| | 
	;      | /   1|    \2|               | /   0|    \ |
	;      |/   0       \|               |/   3       \|
	;      'ÿÿÿÿÿÿÿÿÿÿÿÿÿ'               'ÿÿÿÿÿÿÿÿÿÿÿÿÿ'  
	;             |                             |

	dc.w	5,3
	dc.w	0,3,2,1,0

	dc.w	0,3
	dc.w	0,1,5,4,0

	dc.w	1,3
	dc.w	1,2,6,5,1

	dc.w	2,3
	dc.w	2,3,7,6,2

	dc.w	3,3
	dc.w	0,4,7,3,0

	dc.w	4,3
	dc.w	4,5,6,7,4

; /////////////////////////////////////////////////////////////////////////////
	SECTION	BSS
; /////////////////////////////////////////////////////////////////////////////

; --- BSS FLAG BUFFER ---

; --- BSS DATA BUFFER ---

GURO_BDB_DESKT_MODE:	ds.w	1
GURO_BDB_DESKT_SCREEN:	ds.l	2

GURO_BDB_SCREEN_MODE:	ds.w	1
GURO_BDB_SCREEN_SIZE:	ds.l	1

GURO_BDB_L0006:	ds.w	1
GURO_BDB_L000F:	ds.w	2

GURO_BDB_L0031:	ds.l	200
GURO_BDB_L0032:	ds.l	200

GURO_BDB_L0033:	ds.l	201
GURO_BDB_L0034:	ds.l	201
GURO_BDB_L0035:	ds.l	201
GURO_BDB_L0036:	ds.l	201

GURO_BDB_L0037:	ds.w	1
GURO_BDB_L0038:	ds.w	1

GURO_BDB_SCREEN_MEMORY	ds.b	1
GURO_BDB_SCREEN_FLIP_0	ds.l	1
GURO_BDB_SCREEN_FLIP_1	ds.l	1

GURO_BDB_CORNER_TABLE:	ds.w	6*38	; lI.w,cX.w,cY.w

GURO_BDB_COSIN_TABLE:	ds.w	6*64
	; DATA 383,90,64
	; DIM VTAB&(64)
	; FOR A&=V1& TO 63*V1& STEP V2&
	;   VTAB&(A&)=INT(SIN(A&*(VC&/VD&))*256)*V3&
	; NEXT A&

GURO_BDB_LIN6_TABLE:	ds.w	200
	; DIM VTAB&(200)
	; FOR A&=0 TO 199
	;   VTAB&(A&)=A&*6
	; NEXT A&

GURO_BDB_L0029_TABLE:	ds.w	7*64
	; DATA 6,31
	; DATA 0,0,2048
	; DATA 0,0,2049
	; DATA 0,32,0
	; DATA 0,32,2048
	; DATA 0,0,1
	; DATA 31,32,2048
	; DATA 0,32,2049
	; DIM VTAB&(64)
	; VX&=V1&
	; FOR A&=0 TO 31
	;   VTAB&(A&)=VX&
	;   VX&=+V2&
	;   VTAB&(A&+1)=VX&
	;   VX&=+V2&
	;   VX&=+V3&
	; NEXT A&

GURO_BDB_LIN640_TABLE:	ds.l	200
	; DIM GURO_BDB_LIN640_TABLE(200)
	; FOR A%=0 to 199
	;   GURO_BDB_LIN640_TABLE(A%)=A%*640
	; NEXT A%
