//__BIGPEMU_SCRIPT_MODULE__
//__BIGPEMU_META_DESC__		"Touch control support (and more) for Towers 2."
//__BIGPEMU_META_AUTHOR__	"Rich Whitehouse"

#include "bigpcrt.h"
#include "jagregs.h"

#include "touch_common.inl"

static int32_t sOnInputFrameEvent = -1;
static int32_t sOnLoadEvent = -1;
static int32_t sOnVideoEvent = -1;
static int32_t sOnStateSaveEvent = -1;
static int32_t sOnStateLoadEvent = -1;
static int32_t sOnEmuThreadFrameEvent = -1;

#define T2_DEFAULT_ENABLE_TOUCH				1
#define T2_DEFAULT_TOUCH_OFFSET				0
#define T2_DEFAULT_UNCAP_FRAMERATE			0

static int32_t sEnableTouchSettingHandle = -1;
static int sLastEnableTouch = T2_DEFAULT_ENABLE_TOUCH;
static int32_t sTouchOffsetSettingHandle = -1;
static int sLastTouchOffset = T2_DEFAULT_TOUCH_OFFSET;
static int32_t sUncapFramerateSettingHandle = -1;
static int sLastUncapFramerate = T2_DEFAULT_UNCAP_FRAMERATE;

static uint32_t sIsLoaded = 0;

#define T2_SUPPORTED_HASH						0x0F86DF20AF3A5AE4ULL

#define T2_PERS_DATA_VERSION					2

#define T2_MAX_HIT_FRAME_SPAN					6
#define T2_CURSOR_X_ADDR						0x00024038
#define T2_CURSOR_Y_ADDR						0x0002403A
#define T2_CURSOR_VISIBLE_ADDR					0x00023FB6
#define T2_MAP_VISIBLE_ADDR						0x00027B32
#define T2_CURSOR_MIN_X							0
#define T2_CURSOR_MAX_X							303
#define T2_CURSOR_MIN_Y							0
#define T2_CURSOR_MAX_Y							183
#define T2_UPDATE_CURSOR_MAIN_PC				0x0001EB1A
#define T2_PUSHED_FRAME_LIM_PC					0x0001B228

typedef struct
{
	uint32_t mVersion;
	uint32_t mCursorVisible;
	uint64_t mLastUpdateFrame;
	uint64_t mCurrentFrame;
	uint32_t mExtraButtons;
	uint32_t mUsingFirstTouch;
	float mDisplayRect[4];
} TPersData;

static TPersData sPD;

static void t2_reset_pers_data()
{
	memset(&sPD, 0, sizeof(sPD));
	sPD.mVersion = T2_PERS_DATA_VERSION;
}

static uint32_t on_input_frame(const int32_t eventHandle, void *pEventData)
{
	if (!sIsLoaded)
	{
		return;
	}
	
	TBigPEmuInputFrameParams *pParams = (TBigPEmuInputFrameParams *)pEventData;
	if (pParams->mInputCount > 0 && sPD.mCursorVisible && sPD.mUsingFirstTouch)
	{
		TBigPEmuInput *pInput = pParams->mpInputs;
		pInput->mButtons |= sPD.mExtraButtons;
	}
}

static uint32_t t2_cursor_active()
{
	return (!bigpemu_jag_read16(T2_CURSOR_VISIBLE_ADDR) && !bigpemu_jag_read16(T2_MAP_VISIBLE_ADDR)) ? 1 : 0;
}

static void t2_set_cursor_for_touch()
{
	sPD.mUsingFirstTouch = 0;
	sPD.mExtraButtons = 0;
	if (sPD.mCursorVisible && sTouchCount > 0 && sPD.mDisplayRect[1] > 0.0f && t2_cursor_active())
	{
		const float *pDispRect = sPD.mDisplayRect;
		//always use touch 0 for movement
		const TBigPEmuTouchInfo *pTch = &sTrackedTouches[0];
		if (!bigpemu_touch_intersecting_overlay(pTch->mPos, pTch->mSize) && !bigpemu_touch_intersecting_overlay(pTch->mInitialPos, pTch->mSize))
		{
			sPD.mUsingFirstTouch = 1;
			
			int32_t touchPos[2];
			//arbitrarily eyeballed factors for this game
			const float touchScaleBias[4] = { 1.0f, 0.95f, -54.0f, -34.0f };
			touch_pos_to_clamped_jag_pos(touchPos, pTch->mPos, sPD.mDisplayRect, touchScaleBias, 0);
			touchPos[0] = BIGPEMU_MAX(BIGPEMU_MIN(touchPos[0], T2_CURSOR_MAX_X), T2_CURSOR_MIN_X);
			touchPos[1] = BIGPEMU_MAX(BIGPEMU_MIN(touchPos[1] - sLastTouchOffset, T2_CURSOR_MAX_Y), T2_CURSOR_MIN_Y);
			bigpemu_jag_write16(T2_CURSOR_X_ADDR, touchPos[0]);
			bigpemu_jag_write16(T2_CURSOR_Y_ADDR, touchPos[1]);
			const uint32_t buttonBit = (sTouchCount > 1) ? JAG_BUTTON_A : JAG_BUTTON_B;
			sPD.mExtraButtons |= (1 << buttonBit);
		}
	}
}

static void t2_update_cursor_bp(const uint32_t addr)
{
	sPD.mLastUpdateFrame = bigpemu_jag_get_frame_count();
	t2_set_cursor_for_touch();
}

static void t2_pushed_frame_lim_bp(const uint32_t addr)
{
	const uint32_t sp = bigpemu_jag_m68k_get_areg(7);
	if (!sLastUncapFramerate)
	{
		bigpemu_jag_write16(sp, 5);
		bigpemu_jag_write16(sp + 2, 6);
	}
	else
	{
		bigpemu_jag_write16(sp, 3);
		bigpemu_jag_write16(sp + 2, 3);
		bigpemu_jag_blitter_set_excycles(0);
		bigpemu_jag_gpu_set_pipeline_enabled(0);
	}
}

static uint32_t on_sw_loaded(const int eventHandle, void *pEventData)
{
	sIsLoaded = 0;
	const uint64_t hash = bigpemu_get_loaded_fnv1a64();
	if (hash == T2_SUPPORTED_HASH)
	{
		sIsLoaded = 1;
		reset_touches();
		t2_reset_pers_data();
		bigpemu_jag_m68k_bp_add(T2_UPDATE_CURSOR_MAIN_PC, t2_update_cursor_bp);
		bigpemu_jag_m68k_bp_add(T2_PUSHED_FRAME_LIM_PC, t2_pushed_frame_lim_bp);
	}
	return 0;
}

static uint32_t on_video_frame(const int eventHandle, void *pEventData)
{
	if (sIsLoaded)
	{
		bigpemu_get_setting_value(&sLastEnableTouch, sEnableTouchSettingHandle);
		bigpemu_get_setting_value(&sLastTouchOffset, sTouchOffsetSettingHandle);
		bigpemu_get_setting_value(&sLastUncapFramerate, sUncapFramerateSettingHandle);
		
		if (!sLastEnableTouch)
		{
			sTouchCount = 0;
			bigpemu_touch_set_prefer_hidden_elems(0);
		}
		else
		{
			update_touches(sPD.mCurrentFrame);
			const uint64_t hideBits = (1 << BIGPEMU_TOUCHOL_ELEM_DPAD) |
				(1 << BIGPEMU_TOUCHOL_ELEM_BUTTONB) | (1 << BIGPEMU_TOUCHOL_ELEM_BUTTONC);
			const uint64_t hiddenMask = (sPD.mCursorVisible) ? hideBits : 0;
			bigpemu_touch_set_prefer_hidden_elems(hiddenMask);
		}
		bigpemu_drawui_get_virtual_display_rect(sPD.mDisplayRect);
	}
	return 0;
}

static uint32_t on_save_state(const int32_t eventHandle, void *pEventData)
{
	memcpy(pEventData, &sPD, sizeof(sPD));
	return sizeof(sPD);
}

static uint32_t on_load_state(const int32_t eventHandle, void *pEventData)
{
	if (*(int32_t *)pEventData == T2_PERS_DATA_VERSION)
	{
		memcpy(&sPD, pEventData, sizeof(sPD));
	}
	return 0;
}

static uint32_t on_emu_frame(const int eventHandle, void *pEventData)
{
	if (sIsLoaded)
	{
		sPD.mCurrentFrame = bigpemu_jag_get_frame_count();
		sPD.mCursorVisible = (t2_cursor_active() && sPD.mLastUpdateFrame && (sPD.mCurrentFrame - sPD.mLastUpdateFrame) <= T2_MAX_HIT_FRAME_SPAN) ? 1 : 0;
	}
}

void bigp_init()
{
	void *pMod = bigpemu_get_module_handle();
	
	bigpemu_set_module_usage_flags(pMod, BIGPEMU_MODUSAGE_DETERMINISMWARNING);
	
	const int catHandle = bigpemu_register_setting_category(pMod, "Towers 2");
	sEnableTouchSettingHandle = bigpemu_register_setting_bool(catHandle, "Enable touch", T2_DEFAULT_ENABLE_TOUCH);
	sTouchOffsetSettingHandle = bigpemu_register_setting_int_full(catHandle, "Touch offset", T2_DEFAULT_TOUCH_OFFSET, 0, 260, 1);
	sUncapFramerateSettingHandle = bigpemu_register_setting_bool(catHandle, "Uncap framerate", T2_DEFAULT_UNCAP_FRAMERATE);
	
	sOnInputFrameEvent = bigpemu_register_event_input_frame(pMod, on_input_frame);
	sOnLoadEvent = bigpemu_register_event_sw_loaded(pMod, on_sw_loaded);
	sOnVideoEvent = bigpemu_register_event_video_thread_frame(pMod, on_video_frame);
	sOnStateSaveEvent = bigpemu_register_event_save_state(pMod, on_save_state, T2_SUPPORTED_HASH);
	sOnStateLoadEvent = bigpemu_register_event_load_state(pMod, on_load_state, T2_SUPPORTED_HASH);
	sOnEmuThreadFrameEvent = bigpemu_register_event_emu_thread_frame(pMod, on_emu_frame);
	reset_touches();
	t2_reset_pers_data();
	sIsLoaded = 0;
}

void bigp_shutdown()
{
	void *pMod = bigpemu_get_module_handle();
	bigpemu_unregister_event(pMod, sOnInputFrameEvent);
	bigpemu_unregister_event(pMod, sOnLoadEvent);
	bigpemu_unregister_event(pMod, sOnVideoEvent);
	bigpemu_unregister_event(pMod, sOnStateSaveEvent);
	bigpemu_unregister_event(pMod, sOnStateLoadEvent);
	bigpemu_unregister_event(pMod, sOnEmuThreadFrameEvent);
	sOnInputFrameEvent = -1;
	sOnLoadEvent = -1;
	sOnVideoEvent = -1;
	sOnStateSaveEvent = -1;
	sOnStateLoadEvent = -1;
	sOnEmuThreadFrameEvent = -1;
	sEnableTouchSettingHandle = -1;
	sLastEnableTouch = T2_DEFAULT_ENABLE_TOUCH;
	sTouchOffsetSettingHandle = -1;
	sLastTouchOffset = T2_DEFAULT_TOUCH_OFFSET;
	sUncapFramerateSettingHandle = -1;
	sLastUncapFramerate = T2_DEFAULT_UNCAP_FRAMERATE;
}
