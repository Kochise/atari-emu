
*TOS  Ram loader
*Version for direct load at  27. July 2006.
*Added support for Swedish TOS 2.06 on March 9 2008.

* load  file immed.
*P. Putnik 

*Recognises TOS 1.00 German, US & (UK- ?? not tested)
*TOS 1.04 US & UK & German
*TOS 2.06 German
*TOS 2.06 US & UK
*'Super'TOS 2.06 UK patched
*TOS 2.06 Swedish

*Principle of work:

*We need to relocate all 4-byte (long) addresses
* inside ROM-TOS for new position in RAM
*It can be done with table containing all those addresses
*We need separate tables for every TOS version, since diverse languages
*have diverse locations (addresses) of code and datas
*For supporting some 10 TOS versions 10 tables would take
* a lot of space

*but there is a way with much shorter tables:
*All addresses for relocating must be in ROM address range
*so we can seek by it: for instance if ROM is in range
*$E00000-$E3FFFF we can just look high word of address
* is it in range $E0 to $E3
*So will find all addresses to relocate
*But it will find some code, datas too, which are not absolute ROM addresses
*so we need to make tables containing those, not to reloc addresses
*Such tables are much shorter 
*Here they are at end of code with nonr1 - nonr11  labels
*Finding all those addresses is most of work when adding support for new
*TOS (language) version
*P. Putnik 




begin	lea	stack(pc),a7

;First print message

  lea titmes(pc),a0
  bsr pmess

  

;Search for ROM images

search

 *First check is already installed

  pea isins(pc)
      move.w #38,-(sp)
      trap #14
      addq.l #6,sp

  tst.b itisins
  bne apex  *quit silently if already installed


  lea searchm(pc),a0
  bsr pmess


 
*  bne notosf
  

  
;Load ROM TOS header, first 32 byte

  bsr loadh    
  
  tst.w d0
  beq.s which
  cmp.w #1,d0
  beq notosf
  cmp.w #2,d0
  beq loaderr
  
which
  


  bsr lookv
  cmp.w #13,d0
  beq unsupi ;instead notosf
 


  
;get parameter pointer  
  
getpp  mulu #32,d0
  lea  nam1-32(pc),a2 ;precompens.
  add.l d0,a2 *address
  
  move.l a2,selpoint  ;store it
  

	pea	0.w
	move.w	#$20,-(a7)
	trap	#1 *supervisor
	addq.w	#6,a7
 	move.l d0,-(sp)

	
loadit
        clr.w   -(sp)
	pea    fnam(pc)  * filename
	move.w	#$3D,-(a7) *fopen
	trap	#1
	addq.w	#8,a7
	
	move.w	d0,handl *handle
	bmi	suerr 
	
	
	move.l selpoint(pc),a2
	move.l 18(a2),d2  *selected TOS's length
	movea.l	$42E.w,a6 *phystop
	suba.l	d2,a6
 
	move.l	a6,newphy *new phystop
	
	move.w	#$FFFF,-(a7) *resol. stay
	movea.l	a6,a5
	suba.l	#$8000,a5 *better way
	move.l	a5,-(a7)
	move.l	a5,-(a7)
	move.w	#5,-(a7)
	trap	#14
	lea	$C(a7),a7
	
	pea	instm(pc) *Install message
	move.w	#9,-(a7)
	trap	#1
	addq.w	#6,a7
	
	move.l newphy(pc),-(sp)
	move.l selpoint(pc),a2
	move.l 18(a2),d2  *selected TOS's length
	move.l d2,-(sp)
	move.w	handl(pc),-(a7)
	move.w	#$3F,-(a7)
	trap	#1
	lea	$C(a7),a7

	move.l selpoint(pc),a2
	move.l 18(a2),d2  *selected TOS's length

	cmp.l	d2,d0
	bne	suerr *load error

	move.w	handl(pc),-(a7)
	move.w	#$3E,-(a7) *fclose
	trap	#1
	addq.w	#4,a7

  move.l newphy(pc),d0
  move.l d0,a6

	
	move.l 	a6,$42e.w *now change phystop
	
	pea	relom(pc) *relocating message
	move.w	#9,-(a7)
	trap	#1
	addq.w	#6,a7
	
	move.l 	a6,d2
	swap 	d2
	move.l 	selpoint(pc),a2
	move.w 	14(a2),d3  *selected TOS's base high word
 	move.w 	16(a2),d4  *end address high word
 	move.l 	18(a2),d5 *length
	
	sub.w 	d3,d2 *phystop is always on round adress
	

	move.l 	a6,a5
        	move.l 	a6,a4
	add.l 	d5,a5 *end adr.

	move.l 	22(a2),a1 *don't reloc table
    	moveq   	#0,d0
	
reloc	move.w	(a6),d1
	cmp.w	d3,d1
	blt.s	notrom
	cmp.w	d4,d1
	bge.s	notrom
	cmp.l	(a1),d0 *no reloc if equ !
	beq.s	advnonr
	add.w	d2,d1
	move.w	d1,(a6)
	addq.l	#2,a6
	addq.l  #2,d0
notrom	addq.l	#2,a6
        addq.l  #2,d0
	cmp.l	a5,a6
	blt.s	reloc

	move.l	28(a2),$3F8.w  *sign of install version 'T100 - T206'
	move.l	a4,$3FC.w
	
*Ensure restart of new TOS after reset with button (backside)	
	move.l #$31415926,$426.w
	
	move.w 26(a2),d2  *reset entry offset
	ext.l d2
	move.l a4,a5
	add.l d2,a5 *point after resvector
	move.l a5,$42a.w
        move.l #$5555aaaa,$51a.w *This is due TOS 1.00 !
	
	jmp	(a4) *reset via ramtos!
	
advnonr	addq.l	#4,a1
	bra.s	notrom

suerr   lea loaderrt(pc),a0
   bsr pmess


*Back to user

suexit **********


	move.w	#$20,-(a7)
	trap	#1
	addq.w	#6,a7


keywait	bsr.s input

apex	pea	$4C0000
	trap	#1



loaderr  lea loaderrt(pc),a0
    bra.s prim

unsupi lea unsupm(pc),a0
  bra.s prim

*image not found
notosf	lea	imnfm(pc),a0

prim 	bsr.s pmess
  	bra.s keywait

pmess
	pea	(a0)
	move.w	#9,-(a7)
	trap	#1
	addq.w	#6,a7
  	rts

input    move.w #1,-(sp)
        trap #1
        addq.l #2,sp
        rts


*Detecting is already installed in supervisor
isins  cmp.l #$31415926,$426.w
  seq itisins
  rts

itisins dc.w 0  *flag


  
loadh
 
   
 
 
        clr.w error
        clr.w   -(sp)
	pea	fnam(pc)
	move.w	#$3D,-(sp) *fopen
	trap	#1
	addq.w	#8,sp
	
	move.w	d0,handl *handle
	bmi.s	tosimnf *TOS.IMG not found
	
	
	
	pea stack(pc)
	pea 32.w
	move.w	handl(pc),-(a7)
	move.w	#$3F,-(a7)
	trap	#1
	lea	$C(a7),a7

	cmp.l	#32,d0
	bne.s	hloaderr *load error

	move.w	handl(pc),-(a7)
	move.w	#$3E,-(a7) *fclose
	trap	#1
	addq.w	#4,a7
  
        moveq #0,d0
        rts
        
tosimnf moveq #1,d0
  rts
  
hloaderr  moveq #2,d0
  rts          
  
  	
 
lookv *see which version is

  lea stack(pc),a1
  
  cmp.w  #$0100,2(a1)
  beq.s t100
  cmp.w #$0104,2(a1)
  beq.s t104
  cmp.w #$0206,2(a1)
  beq.s t206

notfs  moveq #13,d0 *not found supported 
  rts 

t100 cmp.w #3,$1c(a1) *is german
  bne.s isuk100
  
  lea t1m(pc),a0
  bsr cmess
  moveq #1,d0
  rts

isuk100 cmp.w #7,$1c(a1) *is UK
  bne.s isus100
  
  lea t8m(pc),a0
  bsr cmess
  moveq #8,d0
  rts

isus100 cmp.w #0,$1c(a1) *is US
  bne.s notfs
  
  lea t9m(pc),a0
  bsr cmess
  moveq #9,d0
  rts


t104 cmp.w #7,$1c(a1) *is English
  bne.s ist104us
  
  lea t2m(pc),a0
  bsr cmess
  moveq #2,d0
  rts
  
ist104us cmp.w #0,$1c(a1)
  bne.s isg104

  lea t7m(pc),a0
  bsr cmess
  moveq #7,d0
  rts

isg104 cmp.w #3,$1c(a1)
  bne.s notfs

  lea t10m(pc),a0
  bsr cmess
  moveq #10,d0
  rts

  
t206 cmp.w #3,$1c(a1) *is German
  bne.s ise206
  
  lea t3m(pc),a0
  bsr cmess
  moveq #3,d0
  rts
  
ise206 cmp.w #7,$1c(a1)
  beq.s ispat2

;Is US TOS 2.06
  cmp.w #0,$1c(a1)
  bne.s issw206

  lea t6m(pc),a0
  bsr cmess
  moveq #6,d0
  rts

  
; Is SW TOS 2.06
issw206
  cmp.w #$0D,$1c(a1)
  bne notfs

  lea t11m(pc),a0
  bsr cmess
  moveq #11,d0
  rts



*further test needed for patched version


ispat2    
 
 
        	clr.w 	error
        	clr.w   	-(sp)
	pea	fnam(pc)
	move.w	#$3D,-(sp) *fopen
	trap	#1
	addq.w	#8,sp
	
	move.w	d0,handl *handle
	bmi.s	ferr2 *TOS.IMG not found
	
	
	clr.w -(sp)
	move.w	handl(pc),-(a7)
        	pea 	$1e956
	move.w	#$42,-(a7)    *Fseek
	trap	#1
	lea	$a(a7),a7

	
	pea stack(pc)
	pea 2.w
	move.w	handl(pc),-(a7)
	move.w	#$3F,-(a7)
	trap	#1
	lea	$C(a7),a7

	cmp.l	#2,d0
	bne.s	ferr2 *load error

	move.w	handl(pc),-(a7)
	move.w	#$3E,-(a7) *fclose
	trap	#1
	addq.w	#4,a7
  
        
        cmp.w #$00e0,stack *is patched
        beq.s t2ep
        
  lea t4m(pc),a0
  bsr cmess
  moveq #4,d0
  rts
         
         
t2ep   lea t5m(pc),a0
  bsr cmess
  moveq #5,d0
  rts
         
        
  
ferr2   bra notfs  *for now
            
  
cmess lea founc(pc),a2
  addq.w #1,(a2)
  move.w (a2),d2
  add.w #'0',d2
  
  move.b d2,founp+2
  
  move.l a0,-(sp)
  
  lea founp(pc),a0
  bsr pmess
  
  move.l (sp)+,a0
  bra pmess





newphy ds.l 1

handl	dc.w	0 *place for handle

*Title message
titmes dc.b 13,10,'TOS loader by P. Putnik',0
searchm  dc.b 13,10,'Searching ROM-TOS images...',0
  
founm dc.b 13,10,'Found:',0

t1m  dc.b 'TOS 1.00 German',0
t2m  dc.b 'TOS 1.04 UK',0
t3m  dc.b 'TOS 2.06 German',0
t4m  dc.b 'TOS 2.06 UK',0
t5m  dc.b 39,'Super',39,'TOS 2.06 UK-patched',0
t6m  dc.b 'TOS 2.06 US',0
t7m  dc.b 'TOS 1.04 US',0
t8m  dc.b 'TOS 1.00 UK',0
t9m  dc.b 'TOS 1.00 US',0
t10m  dc.b 'TOS 1.04 German',0
t11m  dc.b 'TOS 2.06 Swedish',0

*Image file name
fnam	DC.B	'RTOS.IMG',0


loaderrt dc.b 13,10,'Error by loading!',0

	
anywam	dc.b 13,10,'One ROM-TOS already installed.'
  dc.b 13,10,'Load anyway ? ',0

*Install message
instm	
 	dc.b 13,10,'Loading...',0


*Relocating message
relom	DC.B  13,' Relocating...',0


	
*image not found message	
imnfm	dc.b 13,10,'ROM-TOS image not found.',0

unsupm dc.b 13,10,'Unsupported (TOS) image file.',0



founp dc.b 13,10,"x: ",0

  even

founc dc.w 0
* dtas ds.l 1
error ds.w 1
ftosp ds.l 1
fscount ds.w 1

*parameter table for each TOS
*FileName-14 bytes
*Base address, end address - just words for compare
*Length - long
*Nonreloc data address - long
*Offset after resetvector - word
*Install sign - long
*Total 32 bytes


;German 1.00
nam1 ds.b 14  ;place for filename
  dc.w $00FC,$00FF
  dc.l $30000
  dc.l nonr1
  dc.w $74
  dc.l $54313030
  
;UK 1.04  
nam2 ds.b 14
  dc.w $00FC,$00FF
  dc.l $30000
  dc.l nonr2
  dc.w $80
  dc.l $54313034

;German 2.06
nam3 ds.b 14
  dc.w $00E0,$00E4
  dc.l $40000
  dc.l nonr3
  dc.w $7e
  dc.l $54323036

;UK 2.06
nam4 ds.b 14
  dc.w $00E0,$00E4
  dc.l $40000
  dc.l nonr4
  dc.w $7e
  dc.l $54323036

;UK supertos 2.06
nam5 ds.b 14
  dc.w $00E0,$00E4
  dc.l $40000
  dc.l nonr5
  dc.w $7e
  dc.l $54323036
  
;US 2.06  
nam6 ds.b 14
  dc.w $00E0,$00E4
  dc.l $40000
  dc.l nonr4  *same as UK TOS 2.06
  dc.w $7e
  dc.l $54323036

;US 1.04
nam7 ds.b 14
  dc.w $00FC,$00FF
  dc.l $30000
  dc.l nonr2  *same as UK TOS 1.04 ???
  dc.w $80
  dc.l $54313034
  
;UK 1.00  
nam8 ds.b 14  ;place for filename
  dc.w $00FC,$00FF
  dc.l $30000
  dc.l nonr6
  dc.w $74
  dc.l $54313030

;US 1.00
nam9 ds.b 14  ;place for filename
  dc.w $00FC,$00FF
  dc.l $30000
  dc.l nonr6
  dc.w $74
  dc.l $54313030

;German 1.04  
nam10 ds.b 14
  dc.w $00FC,$00FF
  dc.l $30000
  dc.l nonr7
  dc.w $80
  dc.l $54313034
  
;Swedish 2.06
nam11  ds.b 14
  dc.w $00E0,$00E4
  dc.l $40000
  dc.l nonr8
  dc.w $7e
  dc.l $54323036  



*relocating table- this are adresses witch are not for reloc!	
;German TOS 1.00
nonr1 	dc.l $17c8,$17d8,$252e,$2566
	dc.l $28e6,$2b9a,$85ba,$a8c8
	dc.l $fbea,$126d2,$1347e,$135ba,$13bb6
	dc.l $13d6c,$14616,$14716,$207dc
	dc.l $20800,$20d3e,$2afbc,$2afc2
	dc.l $2afca,$2be14,0 	
	
;UK TOS 1.04
nonr2	DC.L $117E,$1514,$3BAC,$3E04
	DC.L $3EB2,$55AE,$610C,$8594
	DC.L $88B4,$15C1C,$170D4,$17426
	DC.L $18406,$1ABC2,$251E8,$27298
	DC.L $28898,$29644,$29780,$29B20
	DC.L $29CD6,$2A580,$2A680,0 *23 ENTRY 
	
;German TOS 2.06
nonr3	DC.L	$89A,$13D0
	DC.L	$1BCA,$1D3A
	DC.L	$1D3E,$1DF0
	DC.L	$1DF6,$229C
	DC.L	$622C,$6BD0
	DC.L	$8DA8,$8DCA
	DC.L	$106C0,$13316
	DC.L	$15840,$17B4A
	DC.L	$18C5C,$190FC
	DC.L	$19274,$19D64
	DC.L	$1A96A,$1B244
	DC.L	$1C3FC,$1CA64
	DC.L	$1CCCA,$1CD98
	DC.L	$1CFBE,$1D82A
	DC.L	$20854,$21210
	DC.L	$22BDC,$22BF8
	DC.L	$2416E,$265B8
	DC.L	$266A6,$27248
	DC.L	$2AA0C,$2B042
	DC.L	$2B802,$2B82A
	DC.L	$2BA18,$32842
	DC.L	$33598,$351C0
	DC.L	$3A2E2,$3A47C
	DC.L	$3A60A,$3A60E
	DC.L	$3B6CE,0

	
	
;UK and US TOS 2.06	
nonr4	DC.L	$89A,$13D0
	DC.L	$1BCA,$1D3A
	DC.L	$1D3E,$1DF0
	DC.L	$1DF6,$229C
	DC.L	$61D2,$6B76
	DC.L	$8D4E,$8D70
	DC.L	$10666,$132BC
	DC.L	$157E6,$17AF0
	DC.L	$18C02,$190A2
	DC.L	$1921A,$19D0A
	DC.L	$1A910,$1B1EA
	DC.L	$1C3A2,$1CA0A
	DC.L	$1CC70,$1CD3E
	DC.L	$1CF64,$1D7D0
	DC.L	$207FA,$211B6
	DC.L	$22B82,$22B9E
	DC.L	$24114,$264B6 ;new entry
	DC.L    $26590,$265B8
	DC.L    $271EE,$2A9B2
	DC.L    $2AFE8
	DC.L	$2B7AC,$2B7D8
	DC.L	$32812
	DC.L	$33568,$35190
	DC.L	$3A0A0 ;new entry
	DC.L    $3A23A,$3A3C8
	DC.L	$3A3CC,0 ;48 address
	
	
	
	
;UK 'Super' TOS 2.06	
nonr5	DC.L	$89A,$13D0
	DC.L	$1BCA,$1D3A
	DC.L	$1D3E,$1DF0
	DC.L	$1DF6,$229C
	DC.L	$61D2,$6B76
	DC.L	$8D4E,$8D70
	DC.L	$10666,$132BC
	DC.L	$157E6,$17AF0
	DC.L	$18C02,$190A2
	DC.L	$1921A,$19D0A
	DC.L	$1A910,$1B1EA
	DC.L	$1C3A2,$1CA0A
	DC.L	$1CC70,$1CD3E
	DC.L	$1CF64,$1E1AA
	DC.L    $1E956
	DC.L	$207FA,$211B6
	DC.L	$22B82,$22B9E
	DC.L	$24114,$264B6
	dc.l	$26590,$265B8
	DC.L    $271EE,$2AFE8
	DC.L	$2B7AC,$2B7D8
	DC.L	$32812
	DC.L	$33568,$35190
	DC.L	$3A0A0,$3A23A
	DC.L 	$3A3C8,$3A3CC,0 ;48 entry

;UK and US TOS 1.00
nonr6 	dc.l $17c8,$17d8,$252e,$2566
	dc.l $28e6,$2b40,$8560,$a86e
	dc.l $fb90,$12678,$13424,$13560,$13b5c
	dc.l $13d12,$145bc,$146bc,$20b1a
	dc.l $20b3e,$2107c,$2b308,$2b30e
	dc.l $2b316,$2c160,0 	

;German TOS 1.04
nonr7	DC.L $117E,$1514,$3A4C
	DC.L $3F0C,$5608,$6166,$85EE
	DC.L $890E,$15C76,$1712E,$17480
	DC.L $18438,$1ABF4,$2521A,$272CA
	DC.L $288CA,$29676,$297B2,$29B52
	DC.L $29D08,$2A5B2,$2A6B2,$2EC00,0 *23 ENTRY 


;Swedish TOS 2.06
nonr8 
	DC.L	$89A,$13D0
	DC.L	$1BCA,$1D3A
	DC.L	$1D3E,$1DF0
	DC.L	$1DF6,$229C
	DC.L	$624A,$6BEE
	DC.L	$8DC6,$8DE8
	DC.L	$106DE,$13334
	DC.L	$1585E,$17B68
	DC.L	$18C7A,$1911A
	DC.L	$19292,$19D82
	DC.L	$1A988,$1B262
	DC.L	$1C41A,$1CA82
	DC.L	$1CCE8,$1CDB6
	DC.L	$1CFDC,$1D848
	DC.L	$20872,$2122E
	DC.L	$22BFA,$22C16
	DC.L	$2418C,$265D6
	DC.L	$266C4,$27266
	DC.L	$2AA2A,$2B060
	DC.L	$2B824,$2B850
	DC.L	$32860
	DC.L	$335B6,$351DE
	DC.L	$3A364,$3A4FE
	DC.L	$3A68C,$3A690
	DC.L	$3B750,0



   section bss

selpoint ds.l 1 


   ds.b 500 *place for stack
stack  ds.b 485000 *place for load & screen & decomp

   