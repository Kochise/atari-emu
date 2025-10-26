//__BIGPEMU_SCRIPT_MODULE__
//__BIGPEMU_META_DESC__		"Touch control support for Raiden."
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

#define RAIDEN_DEFAULT_ENABLE_TOUCH				1
#define RAIDEN_DEFAULT_TOUCH_OFFSET				0

static int32_t sEnableTouchSettingHandle = -1;
static int sLastEnableTouch = RAIDEN_DEFAULT_ENABLE_TOUCH;
static int32_t sTouchOffsetSettingHandle = -1;
static int sLastTouchOffset = RAIDEN_DEFAULT_TOUCH_OFFSET;

static uint32_t sIsLoaded = 0;

#define RAIDEN_SUPPORTED_HASH					0x5FA30C19C9DCD87FULL
#define RAIDEN_REVA_HASH						0x54318D23CD4AB190ULL

#define RAIDEN_PERS_DATA_VERSION				1

#define RAIDEN_PRE_UPDATE_VEL_PC				0x0001B99A
#define RAIDEN_POS_X_ADDR						0x00057B42
#define RAIDEN_POS_Y_ADDR						0x00057B44
#define RAIDEN_VEL_X_ADDR						0x00057B4E
#define RAIDEN_VEL_Y_ADDR						0x00057B50
#define RAIDEN_DIR_X_ADDR						0x00057B52
#define RAIDEN_DIR_Y_ADDR						0x00057B56
#define RAIDEN_MIN_X							12
#define RAIDEN_MAX_X							172
#define RAIDEN_MIN_Y							8
#define RAIDEN_MAX_Y							194
#define RAIDEN_MAX_HIT_FRAME_SPAN				8

typedef struct
{
	uint32_t mVersion;
	uint32_t mInGame;
	uint64_t mLastUpdateFrame;
	uint64_t mCurrentFrame;
	uint32_t mExtraButtons;
	uint32_t mUsingFirstTouch;
	float mDisplayRect[4];
} TPersData;

static TPersData sPD;

static void rai_reset_pers_data()
{
	memset(&sPD, 0, sizeof(sPD));
	sPD.mVersion = RAIDEN_PERS_DATA_VERSION;
}

static uint32_t on_input_frame(const int32_t eventHandle, void *pEventData)
{
	if (!sIsLoaded || !sPD.mInGame)
	{
		return;
	}
	
	TBigPEmuInputFrameParams *pParams = (TBigPEmuInputFrameParams *)pEventData;
	if (pParams->mInputCount > 0 && sPD.mInGame && sPD.mUsingFirstTouch)
	{
		TBigPEmuInput *pInput = pParams->mpInputs;
		pInput->mButtons |= sPD.mExtraButtons;
		//spam attack with any active touches
		if (sTouchCount > 0)
		{
			if ((sPD.mCurrentFrame & 3) <= 1)
			{
				pInput->mButtons |= (1 << JAG_BUTTON_B);
				//spam bombs with more than 1 touch
				if (sTouchCount > 1)
				{
					pInput->mButtons |= (1 << JAG_BUTTON_C);
				}
			}
		}
	}
}

static void rai_pre_update_vel_bp(const uint32_t addr)
{
	sPD.mLastUpdateFrame = bigpemu_jag_get_frame_count();
	sPD.mExtraButtons = 0;
	sPD.mUsingFirstTouch = 0;
	if (sTouchCount > 0 && sPD.mDisplayRect[1] > 0.0f)
	{
		const float *pDispRect = sPD.mDisplayRect;
		//always use touch 0 for movement
		const TBigPEmuTouchInfo *pTch = &sTrackedTouches[0];
		if (!bigpemu_touch_intersecting_overlay(pTch->mPos, pTch->mSize) && !bigpemu_touch_intersecting_overlay(pTch->mInitialPos, pTch->mSize))
		{
			sPD.mUsingFirstTouch = 1;
			
			int32_t touchPos[2];
			//arbitrarily eyeballed factors for this game
			const float touchScaleBias[4] = { 1.0f, 1.0f, -66.0f, -40.0f };
			touch_pos_to_clamped_jag_pos(touchPos, pTch->mPos, sPD.mDisplayRect, touchScaleBias, skTouchPosToJagFlag_FixedEdge);
			touchPos[0] = BIGPEMU_MAX(BIGPEMU_MIN(touchPos[0], RAIDEN_MAX_X + 3), RAIDEN_MIN_X - 3);
			touchPos[1] = BIGPEMU_MAX(BIGPEMU_MIN(touchPos[1] - sLastTouchOffset, RAIDEN_MAX_Y + 3), RAIDEN_MIN_Y - 3);
			const int32_t gameX = (int16_t)bigpemu_jag_read16(RAIDEN_POS_X_ADDR);
			const int32_t gameY = (int16_t)bigpemu_jag_read16(RAIDEN_POS_Y_ADDR);
			
			const int32_t kMaxDelta = 9; //would normally be 3 with just the dpad
			const int32_t kPadDelta = 2;
			const int32_t deltaX = BIGPEMU_MAX(BIGPEMU_MIN(touchPos[0] - gameX, kMaxDelta), -kMaxDelta);
			const int32_t deltaY = BIGPEMU_MAX(BIGPEMU_MIN(touchPos[1] - gameY, kMaxDelta), -kMaxDelta);
			if (deltaX <= -kPadDelta)
			{
				sPD.mExtraButtons |= (1 << JAG_BUTTON_L);
			}
			else if (deltaX >= kPadDelta)
			{
				sPD.mExtraButtons |= (1 << JAG_BUTTON_R);
			}
			if (deltaY <= -kPadDelta)
			{
				sPD.mExtraButtons |= (1 << JAG_BUTTON_U);
			}
			else if (deltaY >= kPadDelta)
			{
				sPD.mExtraButtons |= (1 << JAG_BUTTON_D);
			}

			//if there's existing velocity, stomp it with our extra velocity
			if (bigpemu_jag_read16(RAIDEN_VEL_X_ADDR) || bigpemu_jag_read16(RAIDEN_VEL_Y_ADDR))
			{
				bigpemu_jag_write16(RAIDEN_VEL_X_ADDR, deltaX);
				bigpemu_jag_write16(RAIDEN_VEL_Y_ADDR, deltaY);
			}
		}
	}
}

static uint32_t on_sw_loaded(const int eventHandle, void *pEventData)
{
	sIsLoaded = 0;
	const uint64_t hash = bigpemu_get_loaded_fnv1a64();
	if (hash == RAIDEN_SUPPORTED_HASH)
	{
		sIsLoaded = 1;
		reset_touches();
		rai_reset_pers_data();
		bigpemu_jag_m68k_bp_add(RAIDEN_PRE_UPDATE_VEL_PC, rai_pre_update_vel_bp);
	}
	else if (hash == RAIDEN_REVA_HASH)
	{
		printf_notify("This version of Raiden is not supported by the active script.");
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
			const uint64_t hiddenMask = (sPD.mInGame) ? (BIGPEMU_TOUCHOL_MASK_ALL & ~(1ULL << BIGPEMU_TOUCHOL_ELEM_BUTTONPAUSE)) : 0;
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
	if (*(int32_t *)pEventData == RAIDEN_PERS_DATA_VERSION)
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
		sPD.mInGame = (sPD.mLastUpdateFrame && (sPD.mCurrentFrame - sPD.mLastUpdateFrame) <= RAIDEN_MAX_HIT_FRAME_SPAN) ? 1 : 0;
	}
}

void bigp_init()
{
	void *pMod = bigpemu_get_module_handle();
	
	bigpemu_set_module_usage_flags(pMod, BIGPEMU_MODUSAGE_DETERMINISMWARNING);
	
	const int catHandle = bigpemu_register_setting_category(pMod, "Raiden");
	sEnableTouchSettingHandle = bigpemu_register_setting_bool(catHandle, "Enable touch", RAIDEN_DEFAULT_ENABLE_TOUCH);
	sTouchOffsetSettingHandle = bigpemu_register_setting_int_full(catHandle, "Touch offset", RAIDEN_DEFAULT_TOUCH_OFFSET, 0, 260, 1);
	
	sOnInputFrameEvent = bigpemu_register_event_input_frame(pMod, on_input_frame);
	sOnLoadEvent = bigpemu_register_event_sw_loaded(pMod, on_sw_loaded);
	sOnVideoEvent = bigpemu_register_event_video_thread_frame(pMod, on_video_frame);
	sOnStateSaveEvent = bigpemu_register_event_save_state(pMod, on_save_state, RAIDEN_SUPPORTED_HASH);
	sOnStateLoadEvent = bigpemu_register_event_load_state(pMod, on_load_state, RAIDEN_SUPPORTED_HASH);
	sOnEmuThreadFrameEvent = bigpemu_register_event_emu_thread_frame(pMod, on_emu_frame);
	reset_touches();
	rai_reset_pers_data();
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
	sLastEnableTouch = RAIDEN_DEFAULT_ENABLE_TOUCH;
	sTouchOffsetSettingHandle = -1;
	sLastTouchOffset = RAIDEN_DEFAULT_TOUCH_OFFSET;
}
