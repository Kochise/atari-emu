/*********************************************************************
//
// WINX Definitions
// 
*********************************************************************/

#ifndef __WINX__
#define __WINX__


/*** mesages ***/

#define WM_SHADED		22360
#define WM_UNSHADED		22361


/*** wind_get/wind_set ***/
                                    /* get set */
#define WF_WINX			22360		/*  0   -  */
#define WF_WINXCFG		WF_WINX+1	/*  0   0  */
#define WF_DDELAY		WF_WINX+2	/*  0   0  */
#define WF_SHADE		WF_WINX+5	/*  w   w  */
#define WF_STACK		WF_WINX+6	/*  -   w  */
#define WF_TOPALL		WF_WINX+7	/*  -  0/o */
#define WF_BOTTOMALL	WF_WINX+8	/*  -  0/o */
/* -: not available; w: needs created window; o: needs open window */


/*** wind_calc ***/

#define WC_WIN	0x8000


/*** appl_getinfo ***/

#ifndef AGI_WINDOW
#define AGI_WINDOW	11

	#define AGI1_WFTOP			0x0001
	#define AGI1_WFNEWDESK		0x0002
	#define AGI1_WFCOLOR		0x0004
	#define AGI1_WFDCOLOR		0x0008
	#define AGI1_WFOWNER		0x0010
	#define AGI1_WFBEVENT		0x0020
	#define AGI1_WFBOTTOM		0x0040
	#define AGI1_WFICONIFY		0x0080
	#define AGI1_WFUNICONIFY	0x0100

	#define AGI3_SMALLER		0x0001
	#define AGI3_BOTTOMER		0x0002
	#define AGI3_SCBOTTOM		0x0004
	#define AGI3_HOTCLOSE		0x0008

	#define AGI4_UPDCHKSET		1
#endif

#ifndef AGI_MESAG
#define AGI_MESAG	12

	#define AGI1_WMNEWTOP		0x0001
	#define AGI1_WMUNTOPPED		0x0002
	#define AGI1_WMONTOP		0x0004
	#define AGI1_APTERM			0x0008
	#define AGI1_SHUTDOWN		0x0010
	#define AGI1_CHEXIT			0x0020
	#define AGI1_WMBOTTOMED		0x0040
	#define AGI1_WMICONIFY		0x0080
	#define AGI1_WMUNICONIFY	0x0100
	#define AGI1_WMALLICONIFY	0x0200

	#define AGI3_ICONCOORS		0x0001
#endif

#ifndef AGI_WINX
#define AGI_WINX		22360

	#define AGI1_WFWINX			0x0001
	#define AGI1_WFWINXCFG		0x0002
	#define AGI1_WFDDELAY		0x0004
	#define AGI1_WFSHADE		0x0008
	#define AGI1_WFSTACK		0x0010
	#define AGI1_WFTOPALL		0x0020
	#define AGI1_WFBOTTOMALL	0x0040
	#define AGI1_WFKIND			0x0080

	#define AGI2_XWMARROWED		0x0001
	#define AGI2_WMSHADED		0x0002
	#define AGI2_WMUNSHADED		0x0004

	#define AGI3_XWINDCALC		0x0001

	#define AGI4_DPATSUPPORT	0x0001
#endif

#endif
