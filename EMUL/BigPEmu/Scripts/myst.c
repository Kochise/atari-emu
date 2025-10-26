//__BIGPEMU_SCRIPT_MODULE__
//__BIGPEMU_META_DESC__		"Touch control support for Myst."
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

#define MYST_DEFAULT_ENABLE_TOUCH				1
#define MYST_DEFAULT_TOUCH_OFFSET				0

static int32_t sEnableTouchSettingHandle = -1;
static int sLastEnableTouch = MYST_DEFAULT_ENABLE_TOUCH;
static int32_t sTouchOffsetSettingHandle = -1;
static int sLastTouchOffset = MYST_DEFAULT_TOUCH_OFFSET;

static uint32_t sIsLoaded = 0;

#define MYST_SUPPORTED_HASH						0x5F44CEC1A837375CULL

#define MYST_PERS_DATA_VERSION					1

#define MYST_MAX_HIT_FRAME_SPAN					6
#define MYST_CURSOR_X_ADDR						0x0001D5C4
#define MYST_CURSOR_Y_ADDR						0x0001D5C6
#define MYST_CURSOR_MIN_X						14
#define MYST_CURSOR_MAX_X						256
#define MYST_CURSOR_MIN_Y						0
#define MYST_CURSOR_MAX_Y						191
#define MYST_MENU_FLAG_ADDR						0x0001D4C6
#define MYST_FINISH_DSP_UPLOAD_PC				0x0000555C
#define MYST_WRITE_CURSOR_DSP_PC				0x00F1BB54

#define MYST_CLICK_FRAMES						20
#define MYST_CLICK_HOLD_FRAMES					10
#define MYST_CLICK_DIST							16.0f

typedef struct
{
	uint32_t mVersion;
	uint32_t mCursorVisible;
	uint64_t mLastUpdateFrame;
	uint64_t mCurrentFrame;
	uint64_t mTouchFrame;
	float mTouchDist;
	uint32_t mPressFrame;
	float mDisplayRect[4];
} TPersData;

static TPersData sPD;

static void myst_reset_pers_data()
{
	memset(&sPD, 0, sizeof(sPD));
	sPD.mVersion = MYST_PERS_DATA_VERSION;
}

static uint32_t myst_cursor_active()
{
	return (!bigpemu_jag_read16(MYST_MENU_FLAG_ADDR)) ? 1 : 0;
}

static uint32_t on_input_frame(const int32_t eventHandle, void *pEventData)
{
	if (!sIsLoaded)
	{
		return;
	}
	
	TBigPEmuInputFrameParams *pParams = (TBigPEmuInputFrameParams *)pEventData;
	const uint32_t wantButton = (sPD.mPressFrame >= sPD.mCurrentFrame || sTouchCount > 1) ? 1 : 0;
	if (wantButton && pParams->mInputCount > 0)
	{
		TBigPEmuInput *pInput = pParams->mpInputs;
		pInput->mButtons |= (1 << JAG_BUTTON_B);
	}
}

static void myst_set_cursor_for_touch()
{
	if (sPD.mCursorVisible && sPD.mDisplayRect[1] > 0.0f && myst_cursor_active())
	{
		if (sTouchCount > 0)
		{
			const float *pDispRect = sPD.mDisplayRect;
			//always use touch 0 for movement
			const TBigPEmuTouchInfo *pTch = &sTrackedTouches[0];
			if (!bigpemu_touch_intersecting_overlay(pTch->mPos, pTch->mSize) && !bigpemu_touch_intersecting_overlay(pTch->mInitialPos, pTch->mSize))
			{
				int32_t touchPos[2];
				//arbitrarily eyeballed factors for this game
				const float touchScaleBias[4] = { 1.0f, 1.0f, -30.0f, -42.0f };
				touch_pos_to_clamped_jag_pos(touchPos, pTch->mPos, sPD.mDisplayRect, touchScaleBias, 0);
				touchPos[0] = BIGPEMU_MAX(BIGPEMU_MIN(touchPos[0], MYST_CURSOR_MAX_X), MYST_CURSOR_MIN_X);
				touchPos[1] = BIGPEMU_MAX(BIGPEMU_MIN(touchPos[1] - sLastTouchOffset, MYST_CURSOR_MAX_Y), MYST_CURSOR_MIN_Y);
				bigpemu_jag_write16(MYST_CURSOR_X_ADDR, touchPos[0]);
				bigpemu_jag_write16(MYST_CURSOR_Y_ADDR, touchPos[1]);
				sPD.mTouchFrame = sTouchExtra[0].mBeginFrame;
				sPD.mTouchDist = sTouchExtra[0].mMaxDistanceFromInitial;
			}
		}
		else if (sPD.mTouchFrame)
		{
			if ((sPD.mCurrentFrame - sPD.mTouchFrame) <= MYST_CLICK_FRAMES && sPD.mTouchDist <= MYST_CLICK_DIST)
			{
				sPD.mPressFrame = sPD.mCurrentFrame + MYST_CLICK_HOLD_FRAMES;
			}
			sPD.mTouchFrame = 0;
		}
	}
}


static void myst_write_cursor_dsp_bp(const uint32_t addr)
{
	//perform stomped logic
	bigpemu_jag_dsp_curbank_set_reg(29, (bigpemu_jag_dsp_curbank_get_reg(29) & ~(1 << 3)) | (1 << 12));
	bigpemu_jag_dsp_curbank_set_reg(28, bigpemu_jag_read32(bigpemu_jag_dsp_curbank_get_reg(31)) + 2);
	bigpemu_jag_dsp_set_pc(MYST_WRITE_CURSOR_DSP_PC + 8);
	
	sPD.mLastUpdateFrame = bigpemu_jag_get_frame_count();
	myst_set_cursor_for_touch();
}

static void myst_finish_dsp_upload_bp(const uint32_t addr)
{
	bigpemu_jag_inject_risc_bp(MYST_WRITE_CURSOR_DSP_PC, myst_write_cursor_dsp_bp);
}

static uint32_t on_sw_loaded(const int eventHandle, void *pEventData)
{
	sIsLoaded = 0;
	const uint64_t hash = bigpemu_get_loaded_fnv1a64();
	if (hash == MYST_SUPPORTED_HASH)
	{
		sIsLoaded = 1;
		reset_touches();
		myst_reset_pers_data();
		bigpemu_jag_m68k_bp_add(MYST_FINISH_DSP_UPLOAD_PC, myst_finish_dsp_upload_bp);
	}
	return 0;
}

static uint32_t on_video_frame(const int eventHandle, void *pEventData)
{
	if (sIsLoaded)
	{
		bigpemu_get_setting_value(&sLastEnableTouch, sEnableTouchSettingHandle);
		bigpemu_get_setting_value(&sLastTouchOffset, sTouchOffsetSettingHandle);
	
		if (!sLastEnableTouch)
		{
			sTouchCount = 0;
			bigpemu_touch_set_prefer_hidden_elems(0);
		}
		else
		{
			update_touches(sPD.mCurrentFrame);
			const uint64_t keepButtons = (1ULL << BIGPEMU_TOUCHOL_ELEM_BUTTONOPTION) | (1ULL << BIGPEMU_TOUCHOL_ELEM_BUTTONPAUSE) |
				(1ULL << BIGPEMU_TOUCHOL_ELEM_PAD4) | (1ULL << BIGPEMU_TOUCHOL_ELEM_PAD6);
			const uint64_t hiddenMask = (sPD.mCursorVisible) ? (BIGPEMU_TOUCHOL_MASK_ALL & ~keepButtons) : 0;
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
	if (*(int32_t *)pEventData == MYST_PERS_DATA_VERSION)
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
		sPD.mCursorVisible = (myst_cursor_active() && sPD.mLastUpdateFrame && (sPD.mCurrentFrame - sPD.mLastUpdateFrame) <= MYST_MAX_HIT_FRAME_SPAN) ? 1 : 0;
	}
}

void bigp_init()
{
	void *pMod = bigpemu_get_module_handle();

	bigpemu_set_module_usage_flags(pMod, BIGPEMU_MODUSAGE_DETERMINISMWARNING);
	
	const int catHandle = bigpemu_register_setting_category(pMod, "Myst");
	sEnableTouchSettingHandle = bigpemu_register_setting_bool(catHandle, "Enable touch", MYST_DEFAULT_ENABLE_TOUCH);
	sTouchOffsetSettingHandle = bigpemu_register_setting_int_full(catHandle, "Touch offset", MYST_DEFAULT_TOUCH_OFFSET, 0, 260, 1);
	
	sOnInputFrameEvent = bigpemu_register_event_input_frame(pMod, on_input_frame);
	sOnLoadEvent = bigpemu_register_event_sw_loaded(pMod, on_sw_loaded);
	sOnVideoEvent = bigpemu_register_event_video_thread_frame(pMod, on_video_frame);
	sOnStateSaveEvent = bigpemu_register_event_save_state(pMod, on_save_state, MYST_SUPPORTED_HASH);
	sOnStateLoadEvent = bigpemu_register_event_load_state(pMod, on_load_state, MYST_SUPPORTED_HASH);
	sOnEmuThreadFrameEvent = bigpemu_register_event_emu_thread_frame(pMod, on_emu_frame);
	reset_touches();
	myst_reset_pers_data();
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
	sLastEnableTouch = MYST_DEFAULT_ENABLE_TOUCH;
	sTouchOffsetSettingHandle = -1;
	sLastTouchOffset = MYST_DEFAULT_TOUCH_OFFSET;
}
