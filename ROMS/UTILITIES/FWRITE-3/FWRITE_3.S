	SECTION	TEXT

Start	On s'en va installer

	BRA	Init

	; Les donn‚es du XBRA
	; "XBRA"
	; Nom du XBRA
	; Adresse de l'ancienne routine
	; Nouvelle routine

	DC.B	"XBRA"
	DC.B	"FW-3"
Old_FWRITE	DS.L	1

New_FWRITE	LEA	8(A7),A0

	BTST	#5,(A7)
	BNE	L0003

	MOVE	USP,A0

L0003	CMPI.W	#$40,(A0)
	BNE	Bad_Command

	CMPI.W	#-3,2(A0)
	BNE	Bad_Command

	MOVEM.L	A0-A6/D1-D7,-(A7)
	MOVE.L	4(A0),D7
	MOVE.L	D7,D6
	MOVEA.L	8(A0),A6

L0004	TST.L	D7
	BEQ	L0005

	MOVEQ	#0,D5
	MOVE.B	(A6)+,D5
	MOVE.W	D5,-(A7)
	MOVE.W	#0,-(A7)
	MOVE.W	#3,-(A7)
	TRAP	#$D
	ADDQ.L	#6,A7
	SUBQ.L	#1,D7

	BRA	L0004

L0005	MOVE.L	D6,D0
	MOVEM.L	(A7)+,A0-A6/D1-D7

	RTE

	ILLEGAL

Bad_Command	; La commande ne nous concerne pas
	; On continu sur l'ancienne routine

	MOVEA.L	Old_FWRITE,A0
	JMP	(A0)

Init	; On initialise la m‚moire

	MOVEA.L	A7,A5
	MOVEA.L	4(A5),A5
	MOVE.L	12(A5),D0
	ADD.L	20(A5),D0
	ADD.L	28(A5),D0
	ADDI.L	#$500,D0
	MOVE.L	D0,D1
	ADD.L	A5,D1
	ANDI.L	#-2,D1
	MOVEA.L	D1,A7
	MOVE.L	D0,-(A7)
	MOVE.L	A5,-(A7)
	CLR.W	-(A7)
	MOVE.W	#$4A,-(A7)	; - M_SHRINK -
	TRAP	#1                  	; *** GEMDOS ***
	ADDA.L	#$C,A7

	; On passe en Superviseur pour trifouiller la m‚moire
	
	CLR.L	-(A7)
	MOVE.W	#$20,-(A7)          	; - SUPER -
	TRAP	#1                  	; *** GEMDOS ***
	ADDQ.L	#6,A7

	MOVE.L	D0,Old_Stack
	
	; On v‚rifie que 'lon a un TOS ò 4.00
	
	MOVEA.L	$4F2.L,A0	; D‚but OS
	MOVE.W	2(A0),D0	; Version

	CMP.W	#$400,D0
	BCC	Find_GEMDOS

	; Le TOS est inf‚rieur … 4.00
	
	MOVE.L	#Bad_TOS,-(A7)

	BRA	Print_N_Exit

Find_GEMDOS	; On prend l'adresse actuelle de l'entr‚e GEMDOS

	MOVEA.L	$84.W,A6	; GEMDOS

Verify_Patch	; On regarde si en -12 y'a XBRA
	; Sinon on installe

	CMPI.L	#"XBRA",-12(A6)
	BNE	Install_Patch

	; On regarde si un Patch similaire est d‚ja install‚
	; Sinon on n'installe pas

	CMPI.L	#"_DOS",-8(A6)
	BEQ	Install_Patch

	; On regarde si notre Patch est d‚j… install‚

	CMPI.L	#"FW-3",-8(A6)

	; On prend le XBRA suivant, et on recommence
	; tant que l'on a pas trouv‚ "FW-3"
	
	MOVEA.L	-4(A6),A6
	BNE	Verify_Patch

	MOVE.L	#Patch_OK,-(A7)

Print_N_Exit	MOVE.W	#9,-(A7)	; - C_CONWS -         
	TRAP	#1	; *** GEMDOS ***
	ADDQ.L	#6,A7

	MOVE.L	Old_Stack,-(A7)
	MOVE.W	#$20,-(A7)	; - SUPER -  
	TRAP	#1	; *** GEMDOS ***
	ADDQ.L	#6,A7

	CLR.W	-(A7)	; - P_TERM -
	TRAP	#1	; *** GEMDOS ***

Install_Patch	MOVE.L	$84.L,Old_FWRITE
	MOVE.L	#New_FWRITE,$84.L
	
	MOVE.L	#Patch_Title,-(A7)
	MOVE.W	#9,-(A7)	; - C_CONWS -
	TRAP	#1	; *** GEMDOS ***
	ADDQ.L	#6,A7

	MOVE.L	Old_Stack,-(A7)
  	MOVE.W	#$20,-(A7)	; - SUPER -
	TRAP	#1	; *** GEMDOS ***
	ADDQ.L	#6,A7

	CLR.W	-(A7)
	MOVE.L	#Init,D0
	SUBI.L	#Start,D0
	ADDI.L	#$100,D0
	MOVE.L	D0,-(A7)
	MOVE.W	#$31,-(A7)	; - P_TERMRES -        
	TRAP	#1	; *** GEMDOS ***

	SECTION	DATA

Bad_TOS	DC.B	$0D,$0A,"Le patch FWRITE(-3) est",$0D,$0A
	DC.B	"inutile sur ce TOS.",$0D,$0A
	DC.B	$00,$00

Patch_Title	DC.B	$0D,$0A,$1B,"p                       ",$0D,$0A
	DC.B	"  FWRITE(-3)  BUG-FIX  ",$0D,$0A
	DC.B	" FALCON  TOS 4.02/4.04 ",$0D,$0A
	DC.B	"                       ",$0D,$0A
	DC.B	"  PARX / ATARI France  ",$0D,$0A
	DC.B	"                       ",$0D,$0A
	DC.B	$1B,"q",$0D,$0A,$00,$00

Patch_OK	DC.B	$0D,$0A,$1B,"p FWRITE(-3)  BUG-FIX ",$0D,$0A
	DC.B	" d",$82,"j",$85," en place...    ",$0D,$0A
	DC.B	$1B,"q",$0D,$0A,$00,$00
	
	SECTION	BSS

Old_Stack	DS.L	1
