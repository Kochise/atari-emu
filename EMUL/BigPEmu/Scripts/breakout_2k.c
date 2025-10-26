//__BIGPEMU_SCRIPT_MODULE__
//__BIGPEMU_META_DESC__		"Touch control support for Breakout 2000."
//__BIGPEMU_META_AUTHOR__	"Rich Whitehouse"

#include "bigpcrt.h"
#include "jagregs.h"

#include "touch_common.inl"

static int32_t sOnLoadEvent = -1;
static int32_t sOnVideoEvent = -1;
static int32_t sOnEmuFrameEvent = -1;

#define BREAKOUT_DEFAULT_ENABLE_TOUCH				1

static int32_t sEnableTouchSettingHandle = -1;
static int sLastEnableTouch = BREAKOUT_DEFAULT_ENABLE_TOUCH;

static uint32_t sIsLoaded = 0;
static uint64_t sEmuFrame = 0;
static uint64_t sSetPaddleFrame = 0;

static float sDisplayRect[4] = { 0 };

#define BREAKOUT_SUPPORTED_HASH					0xA1189031055C26CEULL

#define BREAKOUT_UPDATE_PADDLE_PC				0x0000B4C8
#define BREAKOUT_POS_X_ADDR						0x001D19EE
#define BREAKOUT_PADDLE_BASE_ADDR				0x001D1958
#define BREAKOUT_MIN_TOUCH_X					101
#define BREAKOUT_MAX_TOUCH_X					309
#define BREAKOUT_MIN_X							-3232
#define BREAKOUT_MAX_X							3264

static void breakout_update_paddle_bp(const uint32_t addr)
{
	//for now, only care about the first player
	if (bigpemu_jag_m68k_get_areg(0) == BREAKOUT_PADDLE_BASE_ADDR)
	{
		sSetPaddleFrame = bigpemu_jag_get_frame_count();
		if (sTouchCount > 0)
		{
			const TBigPEmuTouchInfo *pTch = &sTrackedTouches[0];
			if (!bigpemu_touch_intersecting_overlay(pTch->mPos, pTch->mSize) && !bigpemu_touch_intersecting_overlay(pTch->mInitialPos, pTch->mSize))
			{
				int32_t touchPos[2];
				const float touchScaleBias[4] = { 1.0f, 1.0f, 0.0f, 0.0f };
				touch_pos_to_clamped_jag_pos(touchPos, pTch->mPos, sDisplayRect, touchScaleBias, 0);

				touchPos[0] = BIGPEMU_MAX(BIGPEMU_MIN(touchPos[0], BREAKOUT_MAX_TOUCH_X), BREAKOUT_MIN_TOUCH_X);
				
				const int32_t newX = BIGPEMU_MAX(BIGPEMU_MIN(BREAKOUT_MIN_X + (touchPos[0] - BREAKOUT_MIN_TOUCH_X) * 32, BREAKOUT_MAX_X), BREAKOUT_MIN_X);
				bigpemu_jag_write16(BREAKOUT_POS_X_ADDR, (int16_t)newX);
			}
		}
	}
}

static uint32_t on_sw_loaded(const int eventHandle, void *pEventData)
{
	sIsLoaded = 0;
	const uint64_t hash = bigpemu_get_loaded_fnv1a64();
	if (hash == BREAKOUT_SUPPORTED_HASH)
	{
		sIsLoaded = 1;
		sSetPaddleFrame = 0;
		reset_touches();
		bigpemu_jag_m68k_bp_add(BREAKOUT_UPDATE_PADDLE_PC, breakout_update_paddle_bp);
	}
	return 0;
}

static uint32_t on_video_frame(const int eventHandle, void *pEventData)
{
	if (sIsLoaded)
	{
		bigpemu_get_setting_value(&sLastEnableTouch, sEnableTouchSettingHandle);		

		//we can be lazy about letting this state be non-deterministic because we're only affecting superficial client state with it
		const int64_t touchFrameDelta = (int64_t)sEmuFrame - (int64_t)sSetPaddleFrame;
		if (!sLastEnableTouch || !sSetPaddleFrame || touchFrameDelta > 30)
		{
			sTouchCount = 0;
			bigpemu_touch_set_prefer_hidden_elems(0);
		}
		else
		{
			update_touches(0);
			const uint64_t showButtons = (1ULL << BIGPEMU_TOUCHOL_ELEM_BUTTONPAUSE) | (1ULL << BIGPEMU_TOUCHOL_ELEM_BUTTONOPTION) |
				(1ULL << BIGPEMU_TOUCHOL_ELEM_BUTTONA) | (1ULL << BIGPEMU_TOUCHOL_ELEM_BUTTONB) | (1ULL << BIGPEMU_TOUCHOL_ELEM_BUTTONC);
			bigpemu_touch_set_prefer_hidden_elems(BIGPEMU_TOUCHOL_MASK_ALL & ~showButtons);
		}
		bigpemu_drawui_get_virtual_display_rect(sDisplayRect);
	}
	return 0;
}

static uint32_t on_emu_frame(const int eventHandle, void *pEventData)
{
	sEmuFrame = bigpemu_jag_get_frame_count();
	return 0;
}

void bigp_init()
{
	void *pMod = bigpemu_get_module_handle();
	
	bigpemu_set_module_usage_flags(pMod, BIGPEMU_MODUSAGE_DETERMINISMWARNING);
	
	const int catHandle = bigpemu_register_setting_category(pMod, "Breakout 2000");
	sEnableTouchSettingHandle = bigpemu_register_setting_bool(catHandle, "Enable touch", BREAKOUT_DEFAULT_ENABLE_TOUCH);
	
	sOnLoadEvent = bigpemu_register_event_sw_loaded(pMod, on_sw_loaded);
	sOnVideoEvent = bigpemu_register_event_video_thread_frame(pMod, on_video_frame);
	sOnEmuFrameEvent = bigpemu_register_event_video_thread_frame(pMod, on_emu_frame);
	reset_touches();
	sIsLoaded = 0;
}

void bigp_shutdown()
{
	void *pMod = bigpemu_get_module_handle();
	bigpemu_unregister_event(pMod, sOnLoadEvent);
	bigpemu_unregister_event(pMod, sOnVideoEvent);
	bigpemu_unregister_event(pMod, sOnEmuFrameEvent);
	sOnLoadEvent = -1;
	sOnVideoEvent = -1;
	sOnEmuFrameEvent = -1;
	sEnableTouchSettingHandle = -1;
	sLastEnableTouch = BREAKOUT_DEFAULT_ENABLE_TOUCH;
}
