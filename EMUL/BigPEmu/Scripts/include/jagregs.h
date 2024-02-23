#pragma once

//conventions copied from the jaguar SDK for convenience.
//however, pointer/dereference defines have been modified to simply reflect the appropriate address.

#define BASE		0xF00000

#define HC    		(BASE+4)
#define VC    		(BASE+6)
#define LPH   		(BASE+8)
#define LPV   		(BASE+0x0A)
#define OB0   		(BASE+0x10)
#define OB1   		(BASE+0x12)
#define OB2   		(BASE+0x14)
#define OB3   		(BASE+0x16)
#define OLP   		(BASE+0x20)
#define OBF   		(BASE+0x26)
#define VMODE 		(BASE+0x28)
#define BORD1 		(BASE+0x2A)
#define BORD2 		(BASE+0x2C)
#define HDB1  		(BASE+0x38)
#define HDB2  		(BASE+0x3A)
#define HDE   		(BASE+0x3C)
#define VS    		(BASE+0x44)
#define VDB   		(BASE+0x46)
#define VDE   		(BASE+0x48)
#define VI    		(BASE+0x4E)
#define PIT0  		(BASE+0x50)
#define PIT1  		(BASE+0x52)
#define BG    		(BASE+0x58)

#define INT1  		(BASE+0xE0)
#define INT2  		(BASE+0xE2)

#define CLUT  		(BASE+0x400)

#define LBUFA 		(BASE+0x800)
#define LBUFB 		(BASE+0x1000)
#define LBUFC 		(BASE+0x1800)

#define BITOBJ		0
#define SCBITOBJ	1
#define GPUOBJ		2
#define BRANCHOBJ	3
#define STOPOBJ		4

#define O_REFLECT	0x00002000
#define O_RMW		0x00004000
#define O_TRANS		0x00008000
#define O_RELEASE	0x00010000

#define O_DEPTH1	(0<<12)
#define O_DEPTH2	(1<<12)
#define O_DEPTH4	(2<<12)
#define O_DEPTH8	(3<<12)
#define O_DEPTH16	(4<<12)
#define O_DEPTH32	(5<<12)

#define O_NOGAP		(1<<15)
#define O_1GAP		(2<<15)
#define O_2GAP		(3<<15)
#define O_3GAP		(4<<15)
#define O_4GAP		(5<<15)
#define O_5GAP		(6<<15)
#define O_6GAP		(7<<15)

#define O_BREQ		(0<<14)
#define O_BRGT		(1<<14)
#define O_BRLT		(2<<14)
#define O_BROP		(3<<14)
#define O_BRHALF	(4<<14)

#define O_STOPINTS	0x00000008


#define NTSC_WIDTH	1409
#define NTSC_HMID	823
#define NTSC_HEIGHT	241
#define NTSC_VMID	266

#define PAL_WIDTH	1381
#define PAL_HMID	843
#define PAL_HEIGHT	287
#define PAL_VMID	322

#define VIDTYPE	0x10

#define VIDEN		0x0001

#define CRY16		0x0000
#define RGB24		0x0002
#define DIRECT16	0x0004
#define RGB16		0x0006

#define GENLOCK		0x0008
#define INCEN		0x0010
#define BINC		0x0020
#define CSYNC		0x0040
#define BGEN		0x0080
#define VARMOD		0x0100

#define PWIDTH1		0x0000
#define PWIDTH2		0x0200
#define PWIDTH3		0x0400
#define PWIDTH4		0x0600
#define PWIDTH5		0x0800
#define PWIDTH6		0x0A00
#define PWIDTH7		0x0C00
#define PWIDTH8		0x0E00  

#define G_FLAGS 	(BASE+0x2100)
#define G_MTXC		(BASE+0x2104)
#define G_MTXA		(BASE+0x2108)
#define G_END		(BASE+0x210C)
#define G_PC		(BASE+0x2110)
#define G_CTRL		(BASE+0x2114)
#define G_HIDATA 	(BASE+0x2118)
#define G_REMAIN 	(BASE+0x211C)
#define G_DIVCTRL 	(BASE+0x211C)
#define G_RAM		(BASE+0x3000)
#define G_ENDRAM	(G_RAM+(4*1024))

#define G_CPUENA	0x00000010
#define G_DSPENA	0x00000020
#define G_PITENA	0x00000040
#define G_OPENA		0x00000080
#define G_BLITENA	0x00000100
#define G_CPUCLR	0x00000200
#define G_DSPCLR	0x00000400
#define G_PITCLR	0x00000800
#define G_OPCLR		0x00001000
#define G_BLITCLR	0x00002000

#define GPUGO		0x00000001
#define GPUINT0		0x00000004

#define G_CPULAT	0x00000040
#define G_DSPLAT	0x00000080
#define G_PITLAT	0x00000100
#define G_OPLAT		0x00000200
#define G_BLITLAT	0x00000400

#define A1_BASE 	(BASE+0x2200)
#define A1_FLAGS	(BASE+0x2204)
#define A1_CLIP		(BASE+0x2208)
#define A1_PIXEL	(BASE+0x220C)
#define A1_STEP		(BASE+0x2210)
#define A1_FSTEP	(BASE+0x2214)
#define A1_FPIXEL	(BASE+0x2218)
#define A1_INC		(BASE+0x221C)
#define A1_FINC		(BASE+0x2220)
#define A2_BASE		(BASE+0x2224)
#define A2_FLAGS	(BASE+0x2228)
#define A2_MASK		(BASE+0x222C)
#define A2_PIXEL	(BASE+0x2230)
#define A2_STEP		(BASE+0x2234)

#define B_CMD		(BASE+0x2238)
#define B_COUNT		(BASE+0x223C)
#define B_SRCD		(BASE+0x2240)
#define B_DSTD		(BASE+0x2248)
#define B_DSTZ		(BASE+0x2250)
#define B_SRCZ1		(BASE+0x2258)
#define B_SRCZ2		(BASE+0x2260)
#define B_PATD		(BASE+0x2268)
#define B_IINC		(BASE+0x2270)
#define B_ZINC		(BASE+0x2274)
#define B_STOP		(BASE+0x2278)

#define B_I3		(BASE+0x227C)
#define B_I2		(BASE+0x2280)
#define B_I1		(BASE+0x2284)
#define B_I0		(BASE+0x2288)

#define B_Z3		(BASE+0x228C)
#define B_Z2		(BASE+0x2290)
#define B_Z1		(BASE+0x2294)
#define B_Z0		(BASE+0x2298)

#define SRCEN		0x00000001
#define SRCENZ		0x00000002
#define SRCENX		0x00000004
#define DSTEN		0x00000008
#define DSTENZ		0x00000010
#define DSTWRZ		0x00000020
#define CLIP_A1		0x00000040
#define UPDA1F		0x00000100
#define UPDA1		0x00000200
#define UPDA2		0x00000400
#define DSTA2		0x00000800
#define GOURD		0x00001000
#define ZBUFF		0x00002000
#define TOPBEN		0x00004000
#define TOPNEN		0x00008000
#define PATDSEL		0x00010000
#define ADDDSEL		0x00020000

#define ZMODELT		0x00040000
#define ZMODEEQ		0x00080000
#define ZMODEGT		0x00100000

#define LFU_NAN		0x00200000
#define LFU_NA		0x00400000
#define LFU_AN		0x00800000
#define LFU_A		0x01000000
#define CMPDST		0x02000000
#define BCOMPEN		0x04000000
#define DCOMPEN		0x08000000
#define BKGWREN		0x10000000
#define BUSHI		0x20000000
#define SRCSHADE	0x40000000

#define LFU_ZERO	0x00000000
#define LFU_NSAND	0x00200000
#define LFU_NSAD	0x00400000
#define LFU_NOTS	0x00600000
#define LFU_SAND	0x00800000
#define LFU_NOTD	0x00A00000
#define LFU_N_SXORD	0x00C00000
#define LFU_NSORND	0x00E00000
#define LFU_SAD		0x01000000
#define LFU_SXORD	0x01200000
#define LFU_D		0x01400000
#define LFU_NSORD	0x01600000
#define LFU_S		0x01800000
#define LFU_SORND	0x01A00000
#define LFU_SORD	0x01C00000
#define LFU_ONE		0x01E00000

#define LFU_REPLACE	0x01800000
#define LFU_XOR		0x01200000
#define LFU_CLEAR	0x00000000

#define PITCH1		0x00000000
#define PITCH2		0x00000001
#define PITCH4		0x00000002
#define PITCH3		0x00000003

#define PIXEL1		0x00000000
#define PIXEL2		0x00000008
#define PIXEL4		0x00000010
#define PIXEL8		0x00000018
#define PIXEL16		0x00000020
#define PIXEL32		0x00000028

#define ZOFFS0		0x00000000
#define ZOFFS1		0x00000040
#define ZOFFS2		0x00000080
#define ZOFFS3		0x000000C0
#define ZOFFS4		0x00000100
#define ZOFFS5		0x00000140
#define ZOFFS6		0x00000180
#define ZOFFS7		0x000001C0

#define WID2		0x00000800
#define WID4 		0x00001000
#define WID6		0x00001400
#define WID8		0x00001800
#define WID10		0x00001A00
#define WID12		0x00001C00
#define WID14		0x00001E00
#define WID16		0x00002000
#define WID20		0x00002200
#define WID24		0x00002400
#define WID28		0x00002600
#define WID32		0x00002800
#define WID40		0x00002A00
#define WID48		0x00002C00
#define WID56		0x00002E00
#define WID64		0x00003000
#define WID80		0x00003200
#define WID96		0x00003400
#define WID112		0x00003600
#define WID128		0x00003800
#define WID160		0x00003A00
#define WID192		0x00003C00
#define WID224		0x00003E00
#define WID256		0x00004000
#define WID320		0x00004200
#define WID384		0x00004400
#define WID448		0x00004600
#define WID512		0x00004800
#define WID640		0x00004A00
#define WID768		0x00004C00
#define WID896		0x00004E00
#define WID1024		0x00005000
#define WID1280		0x00005200
#define WID1536		0x00005400
#define WID1792		0x00005600
#define WID2048		0x00005800
#define WID2560		0x00005A00
#define WID3072		0x00005C00
#define WID3584		0x00005E00

#define XADDPHR		0x00000000
#define XADDPIX		0x00010000
#define XADD0		0x00020000
#define XADDINC		0x00030000

#define YADD0		0x00000000
#define YADD1		0x00040000

#define XSIGNADD	0x00000000
#define XSIGNSUB	0x00080000

#define YSIGNADD	0x00000000
#define YSIGNSUB	0x00100000

#define JPIT1		(BASE+0x10000)
#define JPIT2		(BASE+0x10002)
#define JPIT3		(BASE+0x10004)
#define JPIT4		(BASE+0x10006)

#define J_INT		(BASE+0x10020)
		
#define JOYSTICK	(BASE+0x14000)
#define JOYBUTS		(BASE+0x14002)
#define CONFIG		(BASE+0x14002)

#define MOD_MASK	(BASE+0x1A118)

#define SCLK		(BASE+0x1A150)
#define SMODE		(BASE+0x1A154)

#define L_I2S		(BASE+0x1A148)
#define R_I2S		(BASE+0x1A14C)

#define J_EXTENA	0x0001
#define J_DSPENA	0x0002
#define J_TIM1ENA	0x0004
#define J_TIM2ENA	0x0008
#define J_ASYNENA	0x0010
#define J_SYNENA	0x0020

#define J_EXTCLR	0x0100
#define J_DSPCLR	0x0200
#define J_TIM1CLR	0x0400
#define J_TIM2CLR	0x0800
#define J_ASYNCLR	0x1000
#define J_SYNCLR	0x2000

#define JOY_UP		20
#define JOY_DOWN	21
#define JOY_LEFT	22
#define JOY_RIGHT	23

#define FIRE_A		29
#define FIRE_B		25
#define FIRE_C		13
#define OPTION		9
#define PAUSE		28

#define KEY_STAR	16
#define KEY_7		17
#define KEY_4		18
#define KEY_1		19

#define KEY_0		4
#define KEY_8		5
#define KEY_5		6
#define KEY_2		7

#define KEY_HASH	0
#define KEY_9		1
#define KEY_6		2
#define KEY_3		3

#define ANY_JOY		0x00F00000
#define ANY_FIRE	0x32002200
#define ANY_KEY		0x000F00FF

#define ROM_TABLE  	(BASE+0x1D000)

#define ROM_TRI     (BASE+0x1D000)
#define ROM_SINE	(BASE+0x1D200)
#define ROM_AMSINE	(BASE+0x1D400)
#define ROM_12W		(BASE+0x1D600)
#define ROM_CHIRP16	BASE+0x1D800)
#define ROM_NTRI	(BASE+0x1DA00)
#define ROM_DELTA	(BASE+0x1DC00)
#define ROM_NOISE	(BASE+0x1DE00)

#define D_FLAGS		(BASE+0x1A100)
#define D_MTXC		(BASE+0x1A104)
#define D_MTXA		(BASE+0x1A108)
#define D_END		(BASE+0x1A10C)
#define D_PC		(BASE+0x1A110)
#define D_CTRL		(BASE+0x1A114)
#define D_MOD		(BASE+0x1A118)
#define D_REMAIN	(BASE+0x1A11C)
#define D_DIVCTRL 	(BASE+0x1A11C)
#define D_MACHI		(BASE+0x1A120)
#define D_RAM		(BASE+0x1B000)
#define D_ENDRAM	(D_RAM+(8*1024))

#define D_CPUENA	0x00000010
#define D_I2SENA	0x00000020
#define D_TIM1ENA	0x00000040
#define D_TIM2ENA	0x00000080
#define D_EXT0ENA	0x00000100
#define D_EXT1ENA	0x00010000

#define D_CPUCLR	0x00000200
#define D_I2SCLR	0x00000400
#define D_TIM1CLR	0x00000800
#define D_TIM2CLR	0x00001000
#define D_EXT0CLR	0x00002000
#define D_EXT1CLR	0x00020000

#define DSPGO		0x00000001
#define DSPINT0		0x00000004

#define D_CPULAT	0x00000040
#define D_I2SLAT	0x00000080
#define D_TIM1LAT	0x00000100
#define D_TIM2LAT	0x00000200
#define D_EXT1LAT	0x00000400
#define D_EXT2LAT	0x00010000

#define RISCGO		0x00000001
#define CPUINT		0x00000002
#define FORCEINT0	0x00000004
#define SINGLE_STEP	0x00000008
#define SINGLE_GO	0x00000010

#define REGPAGE		0x00004000
#define DMAEN		0x00008000

#define ZERO_FLAG	0x00000001
#define CARRY_FLAG	0x00000002
#define NEGA_FLAG	0x00000004

#define IMASK		0x00000008

#define DIV_OFFSET	0x00000001
