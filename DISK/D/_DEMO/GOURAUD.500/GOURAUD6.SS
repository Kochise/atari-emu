; /// > GOURAUD SHADED 3D CUBE - KOSS 2000/2001 < ///
	
	OPT	p=68030/68882	; Falcon030 specific
;	OPT	AUTOPC	; Branch to Relative
;	OPT	BDW	; Branch to Word
;	OPT	O+	; All Optimisations
;	OPT	OW-	; No warning
	COMMENT	HEAD=7	; FastLoad
	OUTPUT	GURO6.PRG

; --- EQUATE ASSEMBLY ---

; --- EQUATE DATA ---

GURO_ED_DATE_DAY:	EQU	31	; Binary Date Format
GURO_ED_DATE_MONTH:	EQU	10	;  FEDC BA98 7654 3210
GURO_ED_DATE_YEAR:	EQU	2000	; %YYYY YYYM MMMD DDDD
GURO_ED_DATE_DATE:	EQU	((GURO_ED_DATE_YEAR-1980)<<9)|(GURO_ED_DATE_MONTH<<5)|(GURO_ED_DATE_DAY)

GURO_ED_VIDEO_WIDTH:	EQU	320
GURO_ED_VIDEO_HEIGHT:	EQU	240
GURO_ED_VIDEO_PIXEL:	EQU	2
GURO_ED_VIDEO_SCRNSIZE:	EQU	GURO_ED_VIDEO_WIDTH*GURO_ED_VIDEO_HEIGHT*GURO_ED_VIDEO_PIXEL
GURO_ED_VIDEO_BACKGND:	EQU	0
GURO_ED_VIDEO_FILL:	EQU	0

; --- EQUATE INDEX ---

; --- EQUATE ADDRESS ---

; --- EQUATE SYSTEM ---

L_VIDEL_VSYNC:	EQU	$00000070
L_VIDEL_BACKCOL:	EQU	$FFFF9800
B_VIDEL_SCREEN_H:	EQU	$FFFF8201
B_VIDEL_SCREEN_M:	EQU	$FFFF8203
B_VIDEL_SCREEN_L:	EQU	$FFFF820D
W_VIDEL_OSTNL:	EQU	$FFFF820E
W_VIDEL_VWRAP:	EQU	$FFFF8210
B_VIDEL_STSHIFT:	EQU	$FFFF8260
B_VIDEL_HSR:	EQU	$FFFF8265
W_VIDEL_SPSHIFT:	EQU	$FFFF8266
W_VIDEL_HHT:	EQU	$FFFF8282
W_VIDEL_HBB:	EQU	$FFFF8284
W_VIDEL_HBE:	EQU	$FFFF8286
W_VIDEL_HDB:	EQU	$FFFF8288
W_VIDEL_HDE:	EQU	$FFFF828A
W_VIDEL_HSS:	EQU	$FFFF828C
W_VIDEL_HFS:	EQU	$FFFF828E
W_VIDEL_HEE:	EQU	$FFFF8290
W_VIDEL_VFT:	EQU	$FFFF82A2
W_VIDEL_VBB:	EQU	$FFFF82A4
W_VIDEL_VBE:	EQU	$FFFF82A6
W_VIDEL_VDB:	EQU	$FFFF82A8
W_VIDEL_VDE:	EQU	$FFFF82AA
W_VIDEL_VSS:	EQU	$FFFF82AC
W_VIDEL_RCO:	EQU	$FFFF82C0
W_VIDEL_VCO:	EQU	$FFFF82C2
	
; /////////////////////////////////////////////////////////////////////////////	
	SECTION	TEXT
; /////////////////////////////////////////////////////////////////////////////	

;	BRA	GURO_TUI_START
	
;	dc.b	"ProgHEAD"	; ProgHEADER
;	dc.w	$0100	; ProgVERSION
;	dc.w	GURO_ED_DATE_DATE	; ProgDATE
;	dc.b	"GOURAUD 6",0	; ProgNAME
;	dc.b	"Kochise",0	; ProgAUTHOR
	
; --- TEXT USER INIT ---

GURO_TUI_START:	pea	GURO_TSI_START
	move.w	#$26,-(SP)	; - SUPEXEC -
	TRAP	#14	; *** XBIOS ***
	addq.l	#6,SP
	
; --- TEXT USER MAIN ---

; --- TEXT USER RESTORE ---

; --- TEXT USER EXIT ---

GURO_TUE_EXIT:	clr.w	-(SP)	; - P_TERM0 -
	TRAP	#1	; *** GEMDOS ***
	; END OF PROGRAM -> THE OS REDIRECT YOU TO THE DESKTOP

; --- TEXT USER SUB ---

; --- TEXT SUPER INIT ---------------------------------------------------------

GURO_TSI_START:	dc.w	$A00A	; - LA_HIDEMOUSE -	

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
GURO_TSI_TABLE_SINLOOP:	fmove.x	FP1,FP3
	fsin.x	FP3
	fmul.x	FP2,FP3	; 256
	fmove.w	FP3,(A1)+
	fadd.x	FP0,FP1
	DBF	D0,GURO_TSI_TABLE_SINLOOP

	lea	GURO_BDB_LIN6_TABLE,A0
	move.w	#GURO_ED_VIDEO_HEIGHT,D0
	clr.w	D1
GURO_TSI_TABLE_6LOOP:	move.w	D1,(A0)+
	addq.w	#6,D1
	DBF	D0,GURO_TSI_TABLE_6LOOP

	lea	GURO_DDA_L0029_BANK,A0
	lea	GURO_BDB_L0029_TABLE,A1
	movem.w	(A0)+,D0-D1
GURO_TSI_TABLE_L29INIT:	move.w	D1,D2
	movem.w	(A0)+,D3-D5
GURO_TSI_TABLE_L29LOOP:	move.w	D3,(A1)+
	add.w	D4,D3
	move.w	D3,(A1)+
	add.w	D4,D3
	add.w	D5,D3
	DBF	D2,GURO_TSI_TABLE_L29LOOP	
	DBF	D0,GURO_TSI_TABLE_L29INIT

	lea	GURO_BDB_LINE_TABLE,A0
	move.l	#GURO_ED_VIDEO_HEIGHT,D0
	move.l	#GURO_ED_VIDEO_WIDTH*GURO_ED_VIDEO_PIXEL,D1
	clr.l	D2
GURO_TSI_TABLE_LINLOOP:	move.l	D2,(A0)+
	add.l	D1,D2
	DBF	D0,GURO_TSI_TABLE_LINLOOP

	move.w	W_VIDEL_RCO,D0
	andi.w	#%11,D0
	BEQ	GURO_TSE_EXIT
	lea	([GURO_DAT_VIDEL_REGS.l,D0.w*4]),A0
	move.w	SR,D0
	ori.w	#$0700,SR
	move.l	L_VIDEL_VSYNC,GURO_TSS_XBRA_VSYNC
	move.l	#GURO_TSS_VECT_VSYNC,L_VIDEL_VSYNC
	clr.w	D0
	BSR	GURO_TSS_VIDEL_INIT
	move.w	D0,SR
	clr.w	GURO_BFB_VSYNC

; --- TEXT SUPER MAIN ---
	
GURO_TSM_START:	tst.w	GURO_BFB_VSYNC	; VSYNC
	BEQ	GURO_TSM_START
	clr.w	GURO_BFB_VSYNC
	lea	GURO_DAT_VIDEO_BUFFERS,A0
	movem.l	(A0),D0-D1
	exg	D0,D1	; SWAP SCREEN
	movem.l	D0-D1,(A0)
	andi.b	#%11111100,D0
	move.w	SR,D1
	ori.w	#$0700,SR
	move.b	D0,B_VIDEL_SCREEN_L
	ror.l	#8,D0
	move.b	D0,B_VIDEL_SCREEN_M
	ror.l	#8,D0
	move.b	D0,B_VIDEL_SCREEN_H
	move.w	D1,SR
	
	lea	GURO_DAT_L0023,A0
	movem.l	(A0),D0-D3
	move.l	D3,(A0)+
	move.l	D2,(A0)+
	move.l	D0,(A0)+
	move.l	D1,(A0)+

	BSR	GURO_TSS_CUBE_PLOT
	BSR	GURO_TSS_CUBE_FILL
	BSR	GURO_TSS_CUBE_MASK

	addq.b	#3,GURO_DDA_AXIS_X+1
	addq.b	#2,GURO_DDA_AXIS_Y+1
	addq.b	#1,GURO_DDA_AXIS_Z+1

	cmpi.b	#$39,$FFFFFC02.w	; ACIA - IKBD
	BNE	GURO_TSM_START

; --- TEXT SUPER RESTORE ---
	
	move.w	SR,D0
	ori.w	#$0700,SR
	lea	GURO_BDB_VIDEL_REGS,A0
	move.w	#-1,D0
	BSR	GURO_TSS_VIDEL_INIT
	move.l	GURO_TSS_XBRA_VSYNC,L_VIDEL_VSYNC
	move.w	D0,SR
	
; --- TEXT SUPER EXIT ---

GURO_TSE_EXIT:	RTS

; --- TEXT SUPER SUB ---

	dc.b	"XBRA"
GURO_TSS_XBRA_VSYNC:	ds.l	1
GURO_TSS_VECT_VSYNC:	move.w	#-1,GURO_BFB_VSYNC
	JMP	([GURO_TSS_XBRA_VSYNC])
	
GURO_TSS_VIDEL_INIT:	tst.w	GURO_BFB_VSYNC	; VSYNC
	BEQ	GURO_TSS_VIDEL_INIT
	clr.w	GURO_BFB_VSYNC

	tst.w	D0
	BNE	GURO_TSS_VIDEL_RESTORE
	
	lea	GURO_BDB_VIDEL_REGS,A1
	
	move.w	W_VIDEL_OSTNL,(A1)+
	move.w	W_VIDEL_VWRAP,(A1)+
	move.b	B_VIDEL_STSHIFT,(A1)+
	move.b	B_VIDEL_HSR,(A1)+
	move.w	W_VIDEL_SPSHIFT,(A1)+
	move.w	W_VIDEL_HHT,(A1)+
	move.w	W_VIDEL_HBB,(A1)+
	move.w	W_VIDEL_HBE,(A1)+
	move.w	W_VIDEL_HDE,(A1)+
	move.w	W_VIDEL_HSS,(A1)+
	move.w	W_VIDEL_HFS,(A1)+
	move.w	W_VIDEL_HEE,(A1)+
	move.w	W_VIDEL_VFT,(A1)+
	move.w	W_VIDEL_VBB,(A1)+
	move.w	W_VIDEL_VBE,(A1)+
	move.w	W_VIDEL_VDB,(A1)+
	move.w	W_VIDEL_VDE,(A1)+
	move.w	W_VIDEL_VSS,(A1)+
	move.w	W_VIDEL_RCO,(A1)+
	move.w	W_VIDEL_VCO,(A1)+
	move.l	L_VIDEL_BACKCOL,(A1)+
	clr.w	(A1)+
	move.b	B_VIDEL_SCREEN_H,(A1)+
	move.b	B_VIDEL_SCREEN_M,(A1)+
	move.b	B_VIDEL_SCREEN_L,(A1)+

GURO_TSS_VIDEL_RESTORE:	move.w	W_VIDEL_VCO,D0
	ori.w	#%1100,D0	; STOP
	move.w	D0,W_VIDEL_VCO

	move.w	(A0)+,W_VIDEL_OSTNL
	move.w	(A0)+,W_VIDEL_VWRAP
	move.b	(A0)+,B_VIDEL_STSHIFT
	move.b	(A0)+,B_VIDEL_HSR
	move.w	(A0)+,W_VIDEL_SPSHIFT
	move.w	(A0)+,W_VIDEL_HHT
	move.w	(A0)+,W_VIDEL_HBB
	move.w	(A0)+,W_VIDEL_HBE
	move.w	(A0)+,W_VIDEL_HDE
	move.w	(A0)+,W_VIDEL_HSS
	move.w	(A0)+,W_VIDEL_HFS
	move.w	(A0)+,W_VIDEL_HEE
	move.w	(A0)+,W_VIDEL_VFT
	move.w	(A0)+,W_VIDEL_VBB
	move.w	(A0)+,W_VIDEL_VBE
	move.w	(A0)+,W_VIDEL_VDB
	move.w	(A0)+,W_VIDEL_VDE
	move.w	(A0)+,W_VIDEL_VSS
	move.w	(A0)+,W_VIDEL_RCO
	move.w	(A0)+,W_VIDEL_VCO
	move.l	(A0)+,L_VIDEL_BACKCOL
	move.b	(A0)+,D0
	move.b	(A0)+,B_VIDEL_SCREEN_H
	move.b	(A0)+,B_VIDEL_SCREEN_M
	move.b	(A0)+,D0
	andi.b	#%11111100,D0
	move.b	D0,B_VIDEL_SCREEN_L
	
	RTS

GURO_TSS_CUBE_PLOT:	lea	GURO_DDA_AXIS_BANK,A0
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

GURO_TSS_CUBE_FILL:	lea	GURO_DDA_AXIS_BANK,A0
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
	
	movea.l	GURO_BDB_VIDEO_BUFFER0,A6

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

T_508:	move.l	(GURO_BDB_LINE_TABLE.l,D6.w*4),D0
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

GURO_TSS_CUBE_MASK:	movea.l	GURO_DAT_L0024,A0
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
	move.l	(GURO_BDB_LINE_TABLE.l,D4.w*4),D4
	movea.l	GURO_BDB_VIDEO_BUFFER0,A5
	adda.l	D4,A5

T_2F0:	move.w	(A2)+,D4
	lea	(A5,D4.w*2),A4

	sub.w	(A2)+,D4
	BGE	T_304

	addq.w	#1,D4
	neg.w	D4

T_2FE:	move.w	#GURO_ED_VIDEO_FILL,(A4)+

	DBF	D4,T_2FE

T_304:	lea	(640,A5),A5

	DBF	D1,T_2F0

T_30C:	cmp.w	D5,D7
	BLE	T_35C

	lea	(A0,D6.w*4),A2
	lea	(A1,D6.w*4),A3
	sub.w	D6,D5
	move.l	(GURO_BDB_LINE_TABLE.l,D6.w*4),D6
	movea.l	GURO_BDB_VIDEO_BUFFER0,A5
	adda.l	D6,A5

L0004:	move.w	(A2)+,D0
	move.w	(A3)+,D1
	sub.w	D0,D1

	subq.w	#1,D1
	BLT	T_33E

	lea	(A5,D0.w*2),A4

T_338:	move.w	#GURO_ED_VIDEO_FILL,(A4)+

	DBF	D1,T_338

T_33E:	move.w	(A2)+,D1
	move.w	(A3)+,D0
	sub.w	D0,D1

	subq.w	#1,D1
	BLT	T_352

	lea	(A5,D0.w*2),A4

L0005:	move.w	#GURO_ED_VIDEO_FILL,(A4)+

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
	move.l	(GURO_BDB_LINE_TABLE.l,D2.w*4),D2
	movea.l	GURO_BDB_VIDEO_BUFFER0,A5
	adda.l	D2,A5

T_37A:	move.w	(A2)+,D0
	move.w	(A3)+,D1
	sub.w	D0,D1

	subq.w	#1,D1
	BLT	T_38E

	lea	(A5,D0.w*2),A4

T_388:	move.w	#GURO_ED_VIDEO_FILL,(A4)+

	DBF	D1,T_388

T_38E:	move.w	(A2)+,D1
	move.w	(A3)+,D0
	sub.w	D0,D1

	subq.w	#1,D1
	BLT	T_3A2

	lea	(A5,D0.w*2),A4

T_39C:	move.w	#GURO_ED_VIDEO_FILL,(A4)+

	DBF	D1,T_39C

T_3A2:	lea	(640,A5),A5

	DBF	D3,T_37A

	lea	(A0,D7.w*4),A2
	sub.w	D7,D5
	move.l	(GURO_BDB_LINE_TABLE.l,D7.w*4),D7
	movea.l	GURO_BDB_VIDEO_BUFFER0,A5
	adda.l	D7,A5

T_3C2:	move.w	(A2)+,D7
	lea	(A5,D7.w*2),A4

	sub.w	(A2)+,D7
	BGE	T_3D6

	addq.w	#1,D7
	neg.w	D7

T_3D0:	move.w	#GURO_ED_VIDEO_FILL,(A4)+

	DBF	D7,T_3D0

T_3D6:	lea	(640,A5),A5

	DBF	D5,T_3C2

	RTS

; /////////////////////////////////////////////////////////////////////////////
	SECTION	DATA
; /////////////////////////////////////////////////////////////////////////////

; --- DATA ADDRESS TABLE ---

GURO_DAT_VIDEL_REGS:	ds.l	1	; ST Monochrom
	dc.l	GURO_DAT_VIDEL_RGB_REG	; ST Color
	dc.l	GURO_DAT_VIDEL_VGA_REG	; Falcon030 VGA
	dc.l	GURO_DAT_VIDEL_RGB_REG	; Falcon030 TV

GURO_DAT_VIDEO_BUFFERS:	dc.l	GURO_BDB_VIDEO_BUFFER0
	dc.l	GURO_BDB_VIDEO_BUFFER1

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

GURO_DAT_VIDEL_RGB_REG:	dc.w	0	; OSTNL
	dc.w	320	; VWARP
	dc.b	0	; STSHIFT
	dc.b	0	; HSR
	dc.w	256	; SPSHIFT
	dc.w	254	; HHT
	dc.w	203	; HBB
	dc.w	39	; HBE
	dc.w	46	; HDB
	dc.w	143	; HDE
	dc.w	216	; HSS
	dc.w	0	; HFS
	dc.w	0	; HEE
	dc.w	625	; VFT
	dc.w	613	; VBB
	dc.w	47	; VBE
	dc.w	127	; VDB
	dc.w	527	; VDE
	dc.w	619	; VSS
	dc.w	385	; RCO
	dc.w	0	; VCO
	dc.l	GURO_ED_VIDEO_BACKGND	; BACKCOL
	dc.l	GURO_BDB_VIDEO_BUFFER0	; SCREEN

GURO_DAT_VIDEL_VGA_REG:	dc.w	0	; OSTNL
	dc.w	320	; VWARP
	dc.b	0	; STSHIFT
	dc.b	0	; HSR
	dc.w	256	; SPSHIFT
	dc.w	198	; HHT
	dc.w	141	; HBB
	dc.w	21	; HBE
	dc.w	684	; HDB
	dc.w	145	; HDE
	dc.w	150	; HSS
	dc.w	0	; HFS
	dc.w	0	; HEE
	dc.w	1049	; VFT
	dc.w	1023	; VBB
	dc.w	63	; VBE
	dc.w	63	; VDB
	dc.w	1023	; VDE
	dc.w	1045	; VSS
	dc.w	390	; RCO
	dc.w	5	; VCO
	dc.l	GURO_ED_VIDEO_BACKGND	; BACKCOL
	dc.l	GURO_BDB_VIDEO_BUFFER0	; SCREEN

GURO_DDA_AXIS_BANK:	dc.w	0,0,1002	; aX,aY,aZ

GURO_DDA_AXIS_X:	dc.w	$0020	; rX
GURO_DDA_AXIS_Y:	dc.w	$0020	; rY
GURO_DDA_AXIS_Z:	dc.w	$0000	; rZ

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

	dc.w	-200, -200, -200	; cX,cY,cZ
	dc.w	 200, -200, -200
	dc.w	 200,  200, -200
	dc.w	-200,  200, -200
	dc.w	-200, -200,  200
	dc.w	 200, -200,  200
	dc.w	 200,  200,  200
	dc.w	-200,  200,  200

	; ObjectSHADE

	dc.w	-16, -16, -16	; sX,sY,sZ
	dc.w	 16, -16, -16
	dc.w	 16,  16, -16
	dc.w	-16,  16, -16
	dc.w	-16, -16,  16
	dc.w	 16, -16,  16
	dc.w	 16,  16,  16
	dc.w	-16,  16,  16

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

; --- BSS DATA BUFFER ---

GURO_BDB_VIDEL_REGS:	ds.w	1	; OSTNL
	ds.w	1	; VWARP
	ds.b	1	; STSHIFT
	ds.b	1	; HSR
	ds.w	1	; SPSHIFT
	ds.w	1	; HHT
	ds.w	1	; HBB
	ds.w	1	; HBE
	ds.w	1	; HDB
	ds.w	1	; HDE
	ds.w	1	; HSS
	ds.w	1	; HFS
	ds.w	1	; HEE
	ds.w	1	; VFT
	ds.w	1	; VBB
	ds.w	1	; VBE
	ds.w	1	; VDB
	ds.w	1	; VDE
	ds.w	1	; VSS
	ds.w	1	; RCO
	ds.w	1	; VCO
	ds.l	1	; BACKCOL
	ds.l	1	; SCREEN

	EVEN

GURO_BDB_L0006:	ds.w	1
GURO_BDB_L000F:	ds.w	2

GURO_BDB_L0031:	ds.l	GURO_ED_VIDEO_HEIGHT
GURO_BDB_L0032:	ds.l	GURO_ED_VIDEO_HEIGHT

GURO_BDB_L0033:	ds.l	GURO_ED_VIDEO_HEIGHT+1
GURO_BDB_L0034:	ds.l	GURO_ED_VIDEO_HEIGHT+1
GURO_BDB_L0035:	ds.l	GURO_ED_VIDEO_HEIGHT+1
GURO_BDB_L0036:	ds.l	GURO_ED_VIDEO_HEIGHT+1

GURO_BDB_L0037:	ds.w	1
GURO_BDB_L0038:	ds.w	1

GURO_BDB_CORNER_TABLE:	ds.w	6*38	; lI.w,cX.w,cY.w

GURO_BDB_COSIN_TABLE:	ds.w	6*64
	; DATA 383,90,64
	; DIM GURO_BDB_COSIN_TABLE(64)
	; FOR A=V1 TO 63*V1 STEP V2
	;   GURO_BDB_COSIN_TABLE(A)=INT(SIN(A*(VC/VD))*256)*V3
	; NEXT A

GURO_BDB_LIN6_TABLE:	ds.w	GURO_ED_VIDEO_HEIGHT
	; DIM GURO_BDB_LIN6_TABLE(GURO_ED_VIDEO_HEIGHT)
	; FOR A=0 TO (GURO_ED_VIDEO_HEIGHT-1)
	;   GURO_BDB_LIN6_TABLE(A)=A*6
	; NEXT A

GURO_BDB_L0029_TABLE:	ds.w	7*64
	; DATA 6,31
	; DATA 0,0,2048
	; DATA 0,0,2049
	; DATA 0,32,0
	; DATA 0,32,2048
	; DATA 0,0,1
	; DATA 31,32,2048
	; DATA 0,32,2049
	; DIM GURO_BDB_L0029_TABLE(64)
	; VX=V1
	; FOR A=0 TO 31
	;   GURO_BDB_L0029_TABLE(A)=VX
	;   VX=+V2
	;   GURO_BDB_L0029_TABLE(A+1)=VX
	;   VX=+V2
	;   VX=+V3
	; NEXT A

GURO_BDB_LINE_TABLE:	ds.l	GURO_ED_VIDEO_HEIGHT
	; DIM GURO_BDB_LINE_TABLE(GURO_ED_VIDEO_HEIGHT)
	; FOR A=0 to GURO_ED_VIDEO_HEIGHT-1
	;   GURO_BDB_LINE_TABLE(A)=A*640
	; NEXT A

; --- BSS FLAG BUFFER ---

GURO_BFB_VSYNC:	ds.w	1

GURO_BDB_VIDEO_BUFFER0:	ds.b	GURO_ED_VIDEO_SCRNSIZE+4
GURO_BDB_VIDEO_BUFFER1:	ds.b	GURO_ED_VIDEO_SCRNSIZE+4
