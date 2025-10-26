//__BIGPEMU_SCRIPT_MODULE__
//__BIGPEMU_META_DESC__		"Provides additional functionality for AvP."
//__BIGPEMU_META_AUTHOR__	"Rich Whitehouse"

#include "bigpcrt.h"

static int sOnLoadEvent = -1;
static int sOnVideoEvent = -1;

static int sVBCountSettingHandle = -1;
static int sEnableMouseLookHandle = -1;
static int sMouseLookSensHandle = -1;
static int sStereoEnabledHandle = -1;
static int sStereoSeparationHandle = -1;
static int sStereoWeapOfsHandle = -1;

static uint32_t sOddFrame = 0;
static uint32_t sIsLeftEye = 0;
static uint32_t sStereoIsActive = 0;

#include "avp_common.h"

#define AVP_DEFAULT_VBCOUNT			4
#define AVP_DEFAULT_MOUSELOOK		1
#define AVP_DEFAULT_MOUSESENS		16
#define AVP_DEFAULT_STEREO_ENABLE	0
#define AVP_DEFAULT_STEREO_SEP		32
#define AVP_DEFAULT_STEREO_WEAP_OFS	0

//normally 4 on PAL (16Hz), 5 on NTSC (15Hz)
//a default of 4 under NTSC will mean 20Hz.
static int sLastVBCount = AVP_DEFAULT_VBCOUNT;

static int sLastMouseLook = AVP_DEFAULT_MOUSELOOK;
static int sLastMouseLookSens = AVP_DEFAULT_MOUSESENS;

static int32_t sStereoEnabled = AVP_DEFAULT_STEREO_ENABLE;
static int32_t sStereoSeparation = AVP_DEFAULT_STEREO_SEP;
static int32_t sStereoWeapOfs = AVP_DEFAULT_STEREO_WEAP_OFS;

#define AVP_RAM_CODE_COMPARE_ADDR			0x9022

static const uint8_t skAvpExpectedRamCode[] =
{
	0x30, 0x39, 0x00, 0xF1, 0x40, 0x02, 0xC0, 0x7C, 0x00, 0x10, 0x66, 0x34, 0x33, 0xFC, 0x01, 0x44,
	0x00, 0x02, 0xEE, 0x8C, 0x33, 0xFC, 0x03, 0x4B, 0x00, 0x02, 0xEE, 0x8A, 0x33, 0xFC, 0x00, 0x08,
	0x00, 0x02, 0xEE, 0xA2, 0x33, 0xFC, 0x00, 0x01, 0x00, 0x02, 0xEE, 0xA4, 0x23, 0xFC, 0x00, 0x00,
	0x00, 0x32, 0x00, 0x02, 0xEE, 0x9C, 0x33, 0xFC, 0x00, 0x04, 0x00, 0x02, 0xEE, 0xA0, 0x60, 0x32,
	0x33, 0xFC, 0x01, 0x0C, 0x00, 0x02, 0xEE, 0x8C, 0x33, 0xFC, 0x03, 0x37, 0x00, 0x02, 0xEE, 0x8A,
	0x33, 0xFC, 0x00, 0x04, 0x00, 0x02, 0xEE, 0xA2, 0x33, 0xFC, 0x00, 0x04, 0x00, 0x02, 0xEE, 0xA4,
	0x23, 0xFC, 0x00, 0x00, 0x00, 0x3C, 0x00, 0x02, 0xEE, 0x9C, 0x33, 0xFC, 0x00, 0x05, 0x00, 0x02,
	0xEE, 0xA0, 0x20, 0x3C, 0x00, 0x00, 0x01, 0x40, 0x22, 0x3C, 0x00, 0x00, 0x00, 0xE4, 0x61, 0x00
};

static const uint8_t skAvpExpectedStereoRamCode[] =
{
	0x20, 0x39, 0x00, 0x02, 0xF1, 0xD6, 0x21, 0xC0, 0x7F, 0xB8, 0x21, 0xF9, 0x00, 0x02, 0xF1, 0xB4,
	0x7F, 0x9C, 0x21, 0xF9, 0x00, 0x02, 0xF1, 0xB8, 0x7F, 0xA0, 0x21, 0xF9, 0x00, 0x02, 0xF1, 0xBC,
	0x7F, 0xA4
};

//this is a breakpoint callback function, so it'll be running on the emulator thread.
//this makes all bigpemu_jag_* functions safe to call without synchronization.
static void avp_just_set_vid_params_bp(const uint32_t addr)
{
	//when keeping a breakpoint in any kind of mutable (e.g. ram) memory, it's a good idea to make sure the code we're breaking on is the code we expect to be present.
	//this isn't really necessary in avp's case, but i'm doing it here to establish good practice for anyone using this as an example!
	if (!bigpemu_jag_sysmemcmp(skAvpExpectedRamCode, AVP_RAM_CODE_COMPARE_ADDR, sizeof(skAvpExpectedRamCode)))
	{
		//the (uncapped by hardware performance) framerate is:
		//NTSC - 60 / (vbcount - 1)
		//PAL - 50 / (vbcount - 1)
		printf("AvP: Overwriting vbcount '%i' with '%i'.\n", (int)bigpemu_jag_read16(AVP_VBCOUNT), sLastVBCount);
		//looks legit, now let's stomp the vb count with our own setting
		bigpemu_jag_write16(AVP_VBCOUNT, (uint16_t)sLastVBCount);
		//note that an alternate approach here would be to simply stomp the word at AVP_VBCOUNT with our value every frame via the emulator frame event, which occurs
		//on the emulator thread. this would generally work out just fine, as this memory doesn't seem to ever be used for a different purpose for as long as the game
		//is loaded. however, in taking that approach, it may still be important to only begin the stomping after hitting a "user code" breakpoint to make sure nothing
		//overlapping is stomped during any potential boot/bios/etc. code. this breakpoint would still do well enough to serve such a purpose.
	}
}

static void avp_finish_angles_bp(const uint32_t addr)
{
	if (!sLastMouseLook)
	{
		return;
	}
	
	//check for analog inputs on the first 2 input devices
	for (uint32_t inputIndex = 0; inputIndex < 2; ++inputIndex)
	{
		if (bigpemu_jag_get_device_type(inputIndex) != kBPE_DT_Standard)
		{
			float anal[8];
			if (bigpemu_jag_get_analogs(anal, inputIndex) >= 2)
			{
				uint8_t angle = bigpemu_jag_read8(AVP_CENTER_ANGLE);
				const float analYaw = anal[0];
				if (analYaw)
				{
					const float fSign = (analYaw < 0.0f) ? -1.0f : 1.0f;
					angle -= BIGPEMU_MAX(fabsf(analYaw) * (float)sLastMouseLookSens, 1.0f) * fSign;
					bigpemu_jag_write8(AVP_CENTER_ANGLE, angle);
				}
				break;
			}
		}
	}
}

static uint32_t active_stereo_check()
{
	//we depend on other code locations for this functionality, but we know we have what we're expecting if this code exists
	return (sStereoEnabled && !bigpemu_jag_read8(AVP_IN_PAUSE) && !bigpemu_jag_sysmemcmp(skAvpExpectedStereoRamCode, AVP_PCADDR_COPY_VIEWPOS_A, sizeof(skAvpExpectedStereoRamCode))) ? 1 : 0;
}

static void avp_copy_viewpos_bp(const uint32_t addr, const uint32_t dstAddr)
{
	if (!active_stereo_check())
	{
		return;
	}
	
	const float kAngToRad = (float)0.02463994238109641725490196078432; //normalize and scale to radians
	const float curX = (float)bigpemu_jag_read32(AVP_X_POS);
	const float curY = (float)bigpemu_jag_read32(AVP_Y_POS);
	const float curAng = (float)bigpemu_jag_read8(AVP_CENTER_ANGLE) * kAngToRad;
	const float cs = sinf(curAng);
	const float cc = cosf(curAng);
	const float ofsSign = (sIsLeftEye) ? -1.0f : 1.0f;
	const float ofsDist = (float)sStereoSeparation * 64.0f * ofsSign;
	bigpemu_jag_write32(dstAddr, (int32_t)(curX + cs * ofsDist));
	bigpemu_jag_write32(dstAddr + 4, (int32_t)(curY + cc * ofsDist));
}

static void avp_copy_viewpos_a_bp(const uint32_t addr)
{
	avp_copy_viewpos_bp(addr, AVP_RENDER_VIEW_POS);
}

static void avp_logic_updates_bp(const uint32_t addr)
{
	if (!active_stereo_check())
	{
		return;
	}

	sStereoIsActive = 1;
	sIsLeftEye = (sOddFrame & 1) ? 1 : 0;
	bigpemu_jag_set_stereo_enabled(1);
	bigpemu_jag_set_stereo_scan_eye(sIsLeftEye);
	bigpemu_jag_blitter_set_excycles(0); //make sure the blitter is set to free
	
	//force the vbcount because we're skipping half the logic frames
	bigpemu_jag_write16(AVP_VBCOUNT, 2);

	//only do the logic update for one eye so that we don't desync
	if (sIsLeftEye)
	{
		bigpemu_jag_m68k_set_pc(addr + 0x24);
	}
	sOddFrame ^= 1;
}


static void avp_updated_weap_ol_bp(const uint32_t addr)
{
	if (!active_stereo_check() || !sStereoWeapOfs)
	{
		return;
	}
	
	const uint32_t objAddr = bigpemu_jag_read32(AVP_OBJ_OVER);
	if (objAddr)
	{
		const uint64_t objData = bigpemu_jag_read64(objAddr);
		//avp sets bit 10 on y to hide the object by moving it offscreen
		if (!(objData & 8192))
		{
			const uint64_t p1Data = bigpemu_jag_read64(objAddr + 8);
			const int32_t x = ((int32_t)((p1Data & 4095) << 20) >> 20);
			const int32_t xOffset = (sIsLeftEye) ? -sStereoWeapOfs : sStereoWeapOfs * 2;
			const int32_t newX = x + xOffset;
			bigpemu_jag_write64(objAddr + 8, (p1Data & 0xFFFFFFFFFFFFF000ULL) | (newX & 4095));
		}
	}
}

static void avp_call_overlay_update_bp(const uint32_t addr)
{
	if (!active_stereo_check())
	{
		return;
	}
	
	//keep the view weapon from desync'ing between eyes
	if (!sIsLeftEye)
	{
		avp_updated_weap_ol_bp(addr); //since we're skipping the update for this eye, make sure we still offset the overlay object
		bigpemu_jag_m68k_set_pc(addr + 4);
	}
}

static void avp_kill_stereo(const uint32_t addr)
{
	const uint32_t disabledMode = (sStereoEnabled) ? 3 : 0;
	if (sStereoIsActive != disabledMode)
	{
		bigpemu_jag_write16(AVP_VBCOUNT, (uint16_t)sLastVBCount);
		sStereoIsActive = disabledMode;
		sOddFrame = sIsLeftEye = 0;
		bigpemu_jag_set_stereo_enabled(disabledMode);
	}
}

static void avp_run_new_frame(const uint32_t addr)
{
	//check each frame to kill stereo if necessary (the user may disable the option again at any time)
	if (!active_stereo_check())
	{
		avp_kill_stereo(addr);
	}	
}

static uint32_t on_sw_loaded(const int eventHandle, void *pEventData)
{
	const uint64_t hash = bigpemu_get_loaded_fnv1a64();
	if (hash == AVP_SUPPORTED_HASH)
	{
		//this is the game we're after, so set a breakpoint just after the game has initialized video parameters
		bigpemu_jag_m68k_bp_add(AVP_PCADDR_JUST_SET_PARAMS, avp_just_set_vid_params_bp);
		bigpemu_jag_m68k_bp_add(AVP_PCADDR_FINISH_ANGLES, avp_finish_angles_bp);
		
		bigpemu_jag_m68k_bp_add(AVP_PCADDR_COPY_VIEWPOS_A, avp_copy_viewpos_a_bp);
		bigpemu_jag_m68k_bp_add(AVP_PCADDR_LOGIC_UPDATES, avp_logic_updates_bp);
		bigpemu_jag_m68k_bp_add(AVP_PCADDR_CALL_SCR_OVERLAY_UPDATE, avp_call_overlay_update_bp);
		bigpemu_jag_m68k_bp_add(AVP_PCADDR_SCR_UPDATED_WEAPOL, avp_updated_weap_ol_bp);
		bigpemu_jag_m68k_bp_add(AVP_PCADDR_RUN_NEW_FRAME, avp_run_new_frame);
		bigpemu_jag_m68k_bp_add(AVP_PCADDR_RUN_FRAME_NO_SWAP, avp_run_new_frame);
		bigpemu_jag_m68k_bp_add(AVP_PCADDR_KILLAMBIENT, avp_kill_stereo);
		bigpemu_jag_m68k_bp_add(AVP_PCADDR_UPPER_MAIN_LOOP, avp_kill_stereo);
		bigpemu_jag_m68k_bp_add(AVP_PCADDR_EXIT_MAIN_LOOP, avp_kill_stereo);
		sOddFrame = 0;
	}
	return 0;
}

static uint32_t on_video_frame(const int eventHandle, void *pEventData)
{
	bigpemu_get_setting_value(&sLastVBCount, sVBCountSettingHandle);
	bigpemu_get_setting_value(&sLastMouseLook, sEnableMouseLookHandle);
	bigpemu_get_setting_value(&sLastMouseLookSens, sMouseLookSensHandle);
	bigpemu_get_setting_value(&sStereoEnabled, sStereoEnabledHandle);
	bigpemu_get_setting_value(&sStereoSeparation, sStereoSeparationHandle);
	bigpemu_get_setting_value(&sStereoWeapOfs, sStereoWeapOfsHandle);
	return 0;
}

void bigp_init()
{
	void *pMod = bigpemu_get_module_handle();
	
	bigpemu_set_module_usage_flags(pMod, BIGPEMU_MODUSAGE_DETERMINISMWARNING);
	
	const int catHandle = bigpemu_register_setting_category(pMod, "AvP");
	sVBCountSettingHandle = bigpemu_register_setting_int_full(catHandle, "VB Count", AVP_DEFAULT_VBCOUNT, 1, 6, 1);
	sEnableMouseLookHandle = bigpemu_register_setting_bool(catHandle, "Mouselook", AVP_DEFAULT_MOUSELOOK);
	sMouseLookSensHandle = bigpemu_register_setting_int_full(catHandle, "Sensitivity", AVP_DEFAULT_MOUSESENS, 1, 128, 1);
	sStereoEnabledHandle = bigpemu_register_setting_bool(catHandle, "Stereoscopic Mode", AVP_DEFAULT_STEREO_ENABLE);
	sStereoSeparationHandle = bigpemu_register_setting_int_full(catHandle, "Eye Separation", AVP_DEFAULT_STEREO_SEP, 1, 16384, 1);
	sStereoWeapOfsHandle = bigpemu_register_setting_int_full(catHandle, "Stereo Weapon Offset", AVP_DEFAULT_STEREO_WEAP_OFS, -40, 40, 1);
	
	sOnLoadEvent = bigpemu_register_event_sw_loaded(pMod, on_sw_loaded);
	sOnVideoEvent = bigpemu_register_event_video_thread_frame(pMod, on_video_frame);
}

void bigp_shutdown()
{
	void *pMod = bigpemu_get_module_handle();
	bigpemu_unregister_event(pMod, sOnLoadEvent);
	bigpemu_unregister_event(pMod, sOnVideoEvent);
	sOnLoadEvent = -1;
	sOnVideoEvent = -1;
	sVBCountSettingHandle = -1;
	sEnableMouseLookHandle = -1;
	sMouseLookSensHandle = -1;
	sStereoEnabledHandle = -1;
	sStereoSeparationHandle = -1;
	sStereoWeapOfsHandle = -1;
	sLastVBCount = AVP_DEFAULT_VBCOUNT;
	sLastMouseLook = AVP_DEFAULT_MOUSELOOK;
	sLastMouseLookSens = AVP_DEFAULT_MOUSESENS;
	sStereoEnabled = AVP_DEFAULT_STEREO_ENABLE;
	sStereoSeparation = AVP_DEFAULT_STEREO_SEP;
	sStereoWeapOfs = AVP_DEFAULT_STEREO_WEAP_OFS;
}
