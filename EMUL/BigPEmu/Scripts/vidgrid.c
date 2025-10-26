//__BIGPEMU_SCRIPT_MODULE__
//__BIGPEMU_META_DESC__		"Touch control support for Vid Grid."
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

#define VG_DEFAULT_ENABLE_TOUCH				1

static int32_t sEnableTouchSettingHandle = -1;
static int sLastEnableTouch = VG_DEFAULT_ENABLE_TOUCH;

static uint32_t sIsLoaded = 0;

#define VG_SUPPORTED_HASH						0xFB286CFF629AC4AEULL

#define VG_PERS_DATA_VERSION					1

#define VG_MAX_HIT_FRAME_SPAN					6
#define VG_CURSOR_X_ADDR						0x00F1B3F0
#define VG_CURSOR_Y_ADDR						0x00F1B3F4
#define VG_CURSOR_MIN_X							6
#define VG_CURSOR_MAX_X							341
#define VG_CURSOR_MIN_Y							13
#define VG_CURSOR_MAX_Y							258
#define VG_FINISH_DSP_UPLOAD_PC					0x0001117E
#define VG_UPDATE_CURSOR_MAIN_PC				0x00013B92
#define VG_UPDATE_CURSOR_INGAME_MAIN_PC			0x00018D46

typedef struct
{
	uint32_t mVersion;
	uint32_t mCursorVisible;
	uint32_t mMode;
	uint64_t mLastUpdateFrame;
	uint64_t mCurrentFrame;
	uint32_t mExtraButtons;
	uint32_t mUsingFirstTouch;
	float mDisplayRect[4];
} TPersData;

static TPersData sPD;

static void vg_reset_pers_data()
{
	memset(&sPD, 0, sizeof(sPD));
	sPD.mVersion = VG_PERS_DATA_VERSION;
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

static void vg_updated_cursor_bp(const uint32_t addr)
{
	sPD.mLastUpdateFrame = bigpemu_jag_get_frame_count();
	sPD.mMode = 0;
}

static void vg_updated_cursor_ingame_bp(const uint32_t addr)
{
	sPD.mLastUpdateFrame = bigpemu_jag_get_frame_count();
	sPD.mMode = 1;
}

static void vg_set_cursor_for_touch()
{
	sPD.mUsingFirstTouch = 0;
	sPD.mExtraButtons = 0;
	if (sPD.mCursorVisible && sTouchCount > 0 && sPD.mDisplayRect[1] > 0.0f)
	{
		const float *pDispRect = sPD.mDisplayRect;
		//always use touch 0 for movement
		const TBigPEmuTouchInfo *pTch = &sTrackedTouches[0];
		if (!bigpemu_touch_intersecting_overlay(pTch->mPos, pTch->mSize) && !bigpemu_touch_intersecting_overlay(pTch->mInitialPos, pTch->mSize))
		{
			sPD.mUsingFirstTouch = 1;
			
			int32_t touchPos[2];
			//arbitrarily eyeballed factors for this game
			const float touchScaleBias[4] = { 1.0f, 1.0f, -34.0f, -4.0f };
			touch_pos_to_clamped_jag_pos(touchPos, pTch->mPos, sPD.mDisplayRect, touchScaleBias, 0);
			touchPos[0] = BIGPEMU_MAX(BIGPEMU_MIN(touchPos[0], VG_CURSOR_MAX_X), VG_CURSOR_MIN_X);
			touchPos[1] = BIGPEMU_MAX(BIGPEMU_MIN(touchPos[1], VG_CURSOR_MAX_Y), VG_CURSOR_MIN_Y);
			bigpemu_jag_write32(VG_CURSOR_X_ADDR, touchPos[0]);
			bigpemu_jag_write32(VG_CURSOR_Y_ADDR, touchPos[1]);

			if (sPD.mMode)
			{
				const uint32_t pushButton = (sTouchCount > 1) ? JAG_BUTTON_A : JAG_BUTTON_B;
				sPD.mExtraButtons |= (1 << pushButton);
			}
		}
	}
}

static uint32_t on_sw_loaded(const int eventHandle, void *pEventData)
{
	sIsLoaded = 0;
	const uint64_t hash = bigpemu_get_loaded_fnv1a64();
	if (hash == VG_SUPPORTED_HASH)
	{
		sIsLoaded = 1;
		reset_touches();
		vg_reset_pers_data();
		bigpemu_jag_m68k_bp_add(VG_UPDATE_CURSOR_MAIN_PC, vg_updated_cursor_bp);
		bigpemu_jag_m68k_bp_add(VG_UPDATE_CURSOR_INGAME_MAIN_PC, vg_updated_cursor_ingame_bp);
	}
	return 0;
}

static uint32_t on_video_frame(const int eventHandle, void *pEventData)
{
	if (sIsLoaded)
	{
		bigpemu_get_setting_value(&sLastEnableTouch, sEnableTouchSettingHandle);
	
		if (!sLastEnableTouch)
		{
			sTouchCount = 0;
			bigpemu_touch_set_prefer_hidden_elems(0);
		}
		else
		{
			update_touches(sPD.mCurrentFrame);
			const uint64_t keepButtons = (1ULL << BIGPEMU_TOUCHOL_ELEM_BUTTONOPTION) | (1ULL << JAG_BUTTON_A);
			const uint64_t hiddenMask = (sPD.mCursorVisible && sPD.mMode) ? (BIGPEMU_TOUCHOL_MASK_ALL & ~keepButtons) : 0;
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
	if (*(int32_t *)pEventData == VG_PERS_DATA_VERSION)
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
		sPD.mCursorVisible = (sPD.mLastUpdateFrame && (sPD.mCurrentFrame - sPD.mLastUpdateFrame) <= VG_MAX_HIT_FRAME_SPAN) ? 1 : 0;
		vg_set_cursor_for_touch();
	}
}

void bigp_init()
{
	void *pMod = bigpemu_get_module_handle();
	
	bigpemu_set_module_usage_flags(pMod, BIGPEMU_MODUSAGE_DETERMINISMWARNING);

	const int catHandle = bigpemu_register_setting_category(pMod, "Vid Grid");
	sEnableTouchSettingHandle = bigpemu_register_setting_bool(catHandle, "Enable touch", VG_DEFAULT_ENABLE_TOUCH);
	
	sOnInputFrameEvent = bigpemu_register_event_input_frame(pMod, on_input_frame);
	sOnLoadEvent = bigpemu_register_event_sw_loaded(pMod, on_sw_loaded);
	sOnVideoEvent = bigpemu_register_event_video_thread_frame(pMod, on_video_frame);
	sOnStateSaveEvent = bigpemu_register_event_save_state(pMod, on_save_state, VG_SUPPORTED_HASH);
	sOnStateLoadEvent = bigpemu_register_event_load_state(pMod, on_load_state, VG_SUPPORTED_HASH);
	sOnEmuThreadFrameEvent = bigpemu_register_event_emu_thread_frame(pMod, on_emu_frame);
	reset_touches();
	vg_reset_pers_data();
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
	sLastEnableTouch = VG_DEFAULT_ENABLE_TOUCH;
}
