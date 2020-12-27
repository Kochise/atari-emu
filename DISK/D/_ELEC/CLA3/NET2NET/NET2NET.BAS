'V2r2 -> V2r3 file convertor

DEFINT a-z
LIBRARY"GEMDOS"

DIM SHARED bkstor(32000),junk
DIM SHARED p_osc(50),n_blk,block$(50),n_blk,p_blk
DIM SHARED path$,net_name$,item_name$(4000),dnet$(4000),inet%(4000,5),d_cir%(4000,4),n_inp,ni,n_lnk,lnk$(6000)
DIM SHARED b_pagesize(50,2)
DIM SHARED filetype$

?"CLA .NET FILE PATCHER -> v2r4 format"
?" o Fixes for new style inputs."
?" o Fixes to remove old link/move bug."
?

IF COMMAND$="" THEN
	?"You should give this a .NET file as a command line parameter."
	END
END IF

?"Converting ";COMMAND$

load_net_file COMMAND$

release=VAL(MID$(filetype$,2,1))*10+VAL(RIGHT$(filetype$,1))

SELECT CASE filetype$
="V1R2"
	?"Hmmm. Looks like a v1r2,v2r1 or v2r2 file to me."
="V2R2"
	?"Hmmm. Looks like a v2r2a file to me."
="V2R3"
	?"Hmmm. Looks like a v2r3 file to me."
="V2R4"
	?"Hmmm. Looks like a v2r4 file to me - no changes needed."
	STOP -1
=REMAINDER
	?"I don't seem to recognise this release......sorry."
	STOP -1
END SELECT

'Strip out the inputs in v1r2 ccts

IF n_inp>1 AND release<22 THEN
	?"Stripping out external inputs - you'll have to put them back by hand."
	FOR f=1 TO n_inp-1
		FOR l=1 TO n_lnk-1
			WHILE VAL(MID$(lnk$(l),6,4))=-f AND l<n_lnk
				kill_wire l
			WEND
		NEXT l
	NEXT f
	n_inp=1
END IF

'Fix wire refferances for hierachical IO ports.
IF ni>1 AND release<24 THEN
	?"Fixing hierachical IO ports (solves problems with delete)."
	FOR f=1 TO ni-1
		IF d_cir%(f,0)=11 THEN
			link_id=VAL(RIGHT$(dnet$(f),4))
			IF link_id<>0 THEN
				IF VAL(MID$(lnk$(link_id),6,4))<>f OR MID$(lnk$(link_id),11,4)<>LEFT$(dnet$(f),4) THEN
					dnet$(f)=FNvlo$(f+1)+"00010000"
				END IF
			END IF
		END IF
		IF d_cir%(f,0)=10 THEN
			link_id=VAL(RIGHT$(dnet$(f),4))
			IF link_id<>0 THEN
				IF VAL(MID$(lnk$(link_id),6,4))<>f OR MID$(lnk$(link_id),11,4)<>LEFT$(dnet$(f),4) THEN
					dnet$(f)=FNvlo$(f-1)+"00010000"
				END IF
			END IF
		END IF
	NEXT f
END IF

'Tack on extra blank points to wire links to fix the move bug.
IF n_lnk>1 AND release<23 THEN
	?"Fixing wire links (stops the move bug)."
	FOR f=1 TO n_lnk
		p$=MID$(lnk$(f),23)
		DO WHILE LEN(p$)/8<2
			p$="00000000"+p$
		LOOP
		lnk$(f)=FNvlo$(LEN(p$)/8)+MID$(lnk$(f),5,18)+p$
	NEXT f
END IF

save_net COMMAND$
STOP -1

'==============================
'= SAVE A DESIGN (New style)  =
'=  Written 13/10/92 by Craig =
'==============================
SUB save_net(n$)
SHARED path$,net_name$,item_name$(),dnet$(),inet%(),d_cir%(),n_inp,ni,n_lnk,lnk$()
SHARED p_osc(),bkstor(),n_blk,block$(),n_blk,p_blk
STATIC a%,f%,p$
	p$=n$
	?"Saving '"+n$+"'."
	OPEN "O",#1,n$
'Header
	?#1,"CLA V2R4 CCT"
	?#1,FNvlo$(ni);",";FNvlo$(n_inp);",";FNvlo$(n_lnk);",";
	?#1,FNvlo$(n_blk);",";FNvlo$(p_blk)
'Items Section
	FOR a%=1 TO ni-1
		?#1,item_name$(a%);",";
		FOR f%=0 TO 4
			?#1,FNvlo$(d_cir%(a%,f%));",";
		NEXT f%
		?#1,dnet$(a%)
	NEXT a%
'Inputs Section
	FOR a%=1 TO n_inp-1
		?#1,item_name$(inet%(a%,1));",";
		?#1,FNvlo$(inet%(a%,0));",";
		?#1,FNvlo$(inet%(a%,1));",";
		?#1,FNvlo$(inet%(a%,3));",";
		?#1,FNvlo$(inet%(a%,4))
	NEXT a%
'Wire link paths Section
	FOR a%=1 TO n_lnk-1
		?#1,lnk$(a%)
	NEXT a%
'Blocks Section
	FOR a%=0 TO n_blk-1
		?#1,block$(a%);"PAGESIZE=";FNvlo$(800);":";FNvlo$(600)
	NEXT a%
'Probes Section
	FOR a%=0 TO 19
		?#1,FNvlo$(p_osc(a%));",";
	NEXT a%
	?#1,FNvlo$(p_osc(20))
	CLOSE #1
END SUB

'STRINGIFY NUMERIC
'==================
DEF FNvlo$(BYVAL n%)
	FNvlo$=RIGHT$("0000"+LTRIM$(STR$(n%)),4)
END DEF

'======================================================================
'= Load in a Circuit (.NET) file, identifying the file type (old/new) =
'======================================================================
SUB load_net_file(n$)
SHARED bkstor()
STATIC a$,buf&,a&,fhan%,x,y,w,h
	buf&=VARPTR(bkstor(0))
	fhan%=FNfopen%(n$,0)
	a&=FNfread&(fhan%,12,buf&)
	junk=FNfclose%(fhan%)
	a$=""
	POKEB buf&+12,0
	DO
		a$=a$+CHR$(PEEKB(buf&))
		buf&=buf&+1
	LOOP UNTIL PEEKB(buf&)=0

	IF LEFT$(a$,3)="CLA" THEN
		filetype$=MID$(a$,5,4)
		load_new_type_file(n$)
	ELSE
		?"The file ";n$;" is not a CLA NET file."
		STOP -1
	END IF
	p_blk=0
END SUB

'=============================================================
'= Load in a new-type (13/10/92 onwards) circuit (.NET) file =
'=  Written 13/10/92 by Craig                                =
'=============================================================
SUB load_new_type_file(n$)
SHARED path$,net_name$,item_name$(),dnet$(),inet%(),d_cir%(),n_inp,ni,n_lnk,lnk$()
SHARED p_osc(),bkstor(),n_blk,block$(),n_blk,p_blk
STATIC a%,n$,f%,a$,t$
	OPEN "I",#1,n$
	INPUT #1,a$
'Header
	INPUT#1,a$:ni=VAL(a$)
	INPUT#1,a$:n_inp=VAL(a$)
	INPUT#1,a$:n_lnk=VAL(a$)
	INPUT#1,a$:n_blk=VAL(a$)
	INPUT#1,a$:p_blk=VAL(a$)
'Items Section
	FOR a%=1 TO ni-1
		INPUT#1,item_name$(a%)
		FOR f%=0 TO 4
			INPUT#1,a$
			d_cir%(a%,f%)=VAL(a$)
		NEXT f%
		INPUT#1,dnet$(a%)
	NEXT a%
'Inputs Section
	FOR a%=1 TO n_inp-1
		INPUT#1,t$
		INPUT#1,a$
		inet%(a%,0)=VAL(a$)
		INPUT#1,a$
		inet%(a%,1)=VAL(a$)
		item_name$(inet%(a%,1))=t$
		INPUT#1,a$
		inet%(a%,3)=VAL(a$)
		INPUT#1,a$
		inet%(a%,4)=VAL(a$)
	NEXT a%
'Wire link paths Section
	FOR a%=1 TO n_lnk-1
		INPUT#1,lnk$(a%)
	NEXT a%
'Blocks Section
	FOR a%=0 TO n_blk-1
		INPUT#1,a$
		f%=INSTR(a$,"PAGESIZE=")
		IF f%<>0 THEN
			b_pagesize(a%,0)=VAL(MID$(a$,f%+9,4))
			b_pagesize(a%,1)=VAL(MID$(a$,f%+14,4))
			block$(a%)=LEFT$(a$,LEN(a$)-18)
		ELSE
			b_pagesize(a%,0)=800
			b_pagesize(a%,1)=600
			block$(a%)=a$
		END IF
	NEXT a%
'Probes Section
	IF EOF(1) THEN
		?"WARNING: This is not a standard CLA schematic (no Probes section)."
		?"         It may be a CLA-VHDL generated block ?"
	ELSE
		FOR a%=0 TO 19
			INPUT#1,a$
			p_osc(a%)=VAL(a$)
		NEXT a%
		INPUT#1,a$
		p_osc(20)=VAL(a$)
	END IF
	CLOSE #1
END SUB

'========================================
'= KILL OF AN INTER-COMPONENT LINK WIRE =
'========================================
'= Written 17/5/92,                     =
'=  By Craig Graham.                    =
'========================================
SUB kill_wire(wire_id)
SHARED block$(),n_blk
SHARED dnet$(),inet%(),bkstor(),x_l,y_l,item,ni
SHARED d_cir%(),drw$(),n_inp,lnk$(),n_lnk
STATIC sid,did,w,lstat,n,b$,a%,n$,wid,port_mask,component,l,id,id$,d$,a$
	sid=VAL(MID$(lnk$(wire_id),6,4))
	did=VAL(MID$(lnk$(wire_id),11,4))

	IF wire_id<>0 THEN
		FOR w=wire_id TO n_lnk+1
				lnk$(w)=lnk$(w+1)			' GET RID OF THE WIRE.
		NEXT w
		n_lnk=n_lnk-1
	END IF

	lstat=0

	n$="#"+FNvlo$(wire_id)				' 1:DELETE THE WIRE FROM BLOCKS WHICH CONTAIN IT
	FOR n=0 TO n_blk
		b$=block$(n)
		a%=INSTR(b$,n$)
		IF a%<>0 THEN
			b$=LEFT$(b$,a%-1)+MID$(b$,a%+5)
		END IF
		FOR w=32 TO LEN(b$) STEP 5						'REMAP ANY WIRE id's IN BLOCK DESCRIPTION
			id$=MID$(b$,w,4):id=VAL(id$):a$=MID$(b$,w-1,1)
			IF id>wire_id AND a$="#" THEN
				b$=LEFT$(b$,w-1)+FNvlo$(id-1)+MID$(b$,w+4)
			END IF
		NEXT w
		block$(n)=b$
	NEXT n

	port_mask=NOT(inet%(-sid,3)) AND 3
	d_cir%(did,3)=d_cir%(did,3) AND port_mask

	FOR component=1 TO ni			' 3:REMAP ALL DESTINATION ARRAY WIRE IDs
		l=LEN(dnet$(component))
		IF l<>0 THEN
			FOR w=9 TO l STEP 12
				id=VAL(MID$(dnet$(component),w,4))
				IF id>wire_id THEN
					MID$(dnet$(component),w,4)=FNvlo$(id-1)
				END IF
			NEXT w
		END IF
	NEXT component
END SUB

