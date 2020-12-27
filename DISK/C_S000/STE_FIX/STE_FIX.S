;#################################################################
;#
;#   STE-FIX , ATARI Corporation
;#
;# A reassembled version of STE_FIX for TOS 1.6 with the date
;# 29.07.1989. Now with XBRA and version number 1.1.
;#
;#     Karsten Isakovic
;#
;# The only thing this Atari Patch does, is to wait till the 
;# desktop loads the DESKTOP.INF file and then to patch the first
;#  #E into an #F.
;#

start:	bra	startup 
	dc.b	'XBRA_INF'
OldGemdos:
	dc.l	0	
mytrap1:
	btst.b	#$D,(a7)
	bne	suptrp
	bra	ghead		; No supervisor mode -> continue
suptrp:	tst.w	$59E		; _longframe CPU test
	bne	lstk
	lea	6(a7),a0
	bra	chkfn 
lstk:	lea	8(a7),a0	; parameter 8(SSP)
chkfn:	cmpi.w	#$3D,(a0)	; Fopen 
	bne	ckread
	movea.l	2(a0),a0	; return adress < $E00000
	cmpa.l	#$E00000,a0 
	blt	ghead 		; yes -> continue
	lea	deskinf(pc),a1
	moveq	#$B,d0
tstb:	cmpm.b	(a0)+,(a1)+	; compare the Fopen name with 'DESKTOP.INF' 
	bne	ghead		; no -> continue
	dbf	d0,tstb 
	st	gotopen	; ja   -> yes -> set a flag 
	bra	ghead
;
ckread:	cmpi.w	#$3F,(a0)	; Fread ?
	bne	noread
	tst.b	gotopen		; is the flag set ?
	beq	ghead 
	move.l	8(a0),readbuf	; save the buffer adress
	move.l	2(a7),saveret	; hook to this fread call
	move.l	#mungeit,2(a7) 
noread:	clr.w	gotopen		; clear the flag
ghead:	move.l	OldGemdos(pc),a0
	jmp	(a0)
;
mungeit:move.l	d0,d1		; Fread returns a negative number ?
	blt	xmung		; yes -> continue
	movea.l	readbuf,a0	; buffer with characters read 
getbyt:	cmpi.b	#'#',(a0)+
	bne	nxbyt 
	cmpi.b	#'E',(a0) 
	beq	gote
nxbyt:	dbf	d1,getbyt	; D1 is the number of characters read
	bra	xmung 
gote:	addq.b	#1,6(a0)	; the first '#E' is replaced with '#F'
xmung:	movea.l	saveret,a0
	jmp	(a0)
	illegal 

gotopen:	dc.w	0 
readbuf:	dc.l	0 
saveret:	dc.l	0 
deskinf:	dc.b	'DESKTOP.INF',0 
cookie_j:	ds.l	80	; space for 40 cookies

startup:movea.l	4(a7),a3	; Looks like TurboC code
	movea.l	$C(a3),a0 
	adda.l	$14(a3),a0
	adda.l	$1C(a3),a0
	adda.w	#$100,a0	; real length of basepage
	move.l	a3,d0 
	add.l	a0,d0 
	and.b	#$FC,d0		; set up a stack
	movea.l	d0,a7 
	move.l	a0,-(a7)
	move.l	a3,-(a7)
	move.w	#0,-(a7)          
	move.w	#$4A,-(a7)	; Mshrink tor eal size
	trap	#1
	lea	$C(a7),a7 
	bsr	main
	clr.w	-(a7) 
	move.l	#startup-start,-(a7) 
	move.w	#$31,-(a7)	;Ptermres
	trap	#1 
	illegal 

validate:	
	move.l  a2,-(a7)
	move.l  a3,-(a7)
	clr.w   d0
	moveq   #0,d1 
	lea     errno,a1
	move.w  d0,(a1) 
	movea.l $5A0,a0		; cookie jar pointer
	move.l  a0,d2 
	bne     V_Tst_2
	bra     Not_STE

Tst_2_loop:
	cmpi.l  #"_INF",(a0) 	; search for _INF cookie
	bne     V_GoOn
	move.w  #-1,(a1) 	; 'Already installed' error
	bra	val_end

V_GoOn:	cmpi.l  #"_MCH",(a0) 	; search for _MCH (machine) cookie
	bne     V_Nxt
	cmpi.l  #$10000,4(a0) 
	bne     Not_STE
	moveq   #1,d0 
V_Nxt:	addq.l  #1,d1 
	addq.w  #8,a0 		; next cookie
V_Tst_2:move.l  (a0),d2 
	bne     Tst_2_loop
	tst.w   d0		; found STE cookie ?
	bne     V_Tst_3

Not_STE:move.w  #-2,(a1)	; 'Not STE' error
	bra     val_end

V_Tst_3:movea.l $4F2,a2 		; _sysheader
	cmpi.l  #$07291989,$18(a2)	; 29.07.1989 check the date
	beq     V_Tst_4
	move.w  #-3,(a1) 
	bra     val_end

V_Tst_4:cmp.l   4(a0),d1		; place for more cookies ?
	ble     V_Tst_5			
	move.l  #"_INF",(a0) 		; install new cookie
	move.l  #$10001,4(a0) 		; set the version number to 1.1
	addq.w  #8,a0
	clr.l   (a0)
	moveq   #1,d0
	add.l   d1,d0 
	move.l  d0,4(a0)
	bra     val_end

V_Tst_5:movea.l $5A0,a0 		; copy cookie jar
	lea     cookie_j,a1 
	move.l  a1,$5A0 
	bra     V_Copy

Tst_5_loop:
	movea.l a0,a2
	addq.w  #8,a0 
	movea.l a1,a3 
	addq.w  #8,a1 
	move.l  (a2)+,(a3)+ 		; cookie id
	move.l  (a2)+,(a3)+ 		; cookie value
V_Copy: move.l  (a0),d0
	bne	Tst_5_loop
	move.l	#"_INF",(a1) 		; install _INF cookie
	move.l	#$10001,4(a1) 		; set the version number to 1.1
	addq.w	#8,a1
	clr.l	(a1)
	move.l	#$28,4(a1)
val_end:movea.l	(a7)+,a3
	movea.l	(a7)+,a2
	rts 
	
main:   lea	validate(pc),a0	; check OS and 'already installed'
	jsr     Supexec(pc) 
	lea     CrLf,a0 
	jsr     Cconws(pc)
	move.w  errno,d0
	beq     NoError
	lea     Error,a0 
	jsr     Cconws(pc)
NoError:move.w  errno,d0	; write the 'errno' message
	neg.w   d0
	lsl.w   #2,d0 
	lea     msg,a0
	movea.l 0(a0,d0),a0 
	jsr     Cconws(pc)
	lea     EndMsg,a0 
	jsr     Cconws(pc)
	move.w  errno,d0
	beq     Install
	jsr     Pterm0(pc)	; does not return...
	
Install:lea     mytrap1(pc),a0
	moveq   #$21,d0		; hook gemdos trap with XBRA
	jsr     Setexc(pc)
	move.l  a0,OldGemdos	; 
	rts 
;
; This is stuff from the TurboC library.
;
Pterm0: move.w  #0,-(a7) 
	trap    #1 
;
Cconws: move.l  a0,-(a7)
	move.w  #9,-(a7)
	trap    #1
	addq.w  #6,a7 
	rts 
;
Setexc:	move.l  a2,-(a7)
	move.l  a0,-(a7)
	move.w  d0,-(a7)
	move.w  #5,-(a7)
	trap    #$D 
	addq.w  #8,a7 
	movea.l d0,a0 
	movea.l (a7)+,a2
	rts 
;
Supexec:move.l  a2,-(a7)
	move.l  a0,-(a7)
	move.w  #$26,-(a7) 	
	trap    #$E 
	addq.w  #6,a7 
	movea.l (a7)+,a2
	rts 
;
msg:	dc.l	msg_a,err_1,err_2,err_3
msg_a:	dc.b	'DESKTOP.INF 1.1 patch installed',0
err_1:	dc.b	'already installed',0 
err_2:	dc.b	'this machine is not an STE',0
err_3:	dc.b	'this STE ROM version does not need the patch',0 

CrLf:	dc.b	$D,$A,0 
Error:
L10423:	dc.b	'ERROR: ',0 
EndMsg:	dc.b	'.',$D,$A,0,0

	.bss 

errno:	ds.b	$1002

