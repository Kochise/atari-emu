//__BIGPEMU_SCRIPT_MODULE__
//__BIGPEMU_META_DESC__		"Turbo/rapid fire functionality, using a bindable modifier button to toggle turbo mode for a given emulated system button."
//__BIGPEMU_META_AUTHOR__	"Rich Whitehouse"

#include "bigpcrt.h"

static int sOnInputFrameEvent = -1;
static int sOnVideoFrameEvent = -1;
static int sOnPostUIEvent = -1;
static int sOnLoadEvent = -1;

static int32_t sTapIntervalHandle = -1;
static int32_t sAutoTapHandle = -1;
static int32_t sAutoHoldHandle = -1;
static int32_t sSetTurboModHandle = -1;

static int32_t sUserOLElemIndex = -1;

#define TURBO_CONFIG_BLOB_NAME "TurboJoy_ConfigBlob"

#define TURBO_TAP_INTERVAL_DEFAULT 5
#define AUTO_TAP_DEFAULT 0
#define AUTO_HOLD_DEFAULT 0

#define MAX_INPUT_DEVICE_COUNT 8
#define MAX_EMU_BUTTON_COUNT 32
#define INPUT_BUFFER_SIZE 256
#define MAX_HK_HELD_COUNT 4
static uint8_t sInputBuffer[INPUT_BUFFER_SIZE];
static uint32_t sInputSize = 0;
static uint32_t sMaxInputsInBuffer = 0;

static uint32_t sInModBindMode = 0;
static uint32_t sModBindState = 0;
static uint8_t sBindInputBuffer[INPUT_BUFFER_SIZE];
static uint32_t sBindHeldCount = 0;

static uint32_t sWantInitialBindDisplay = 0;

#define TURBO_SCRIPT_VERSION 1
typedef struct
{
	uint32_t mScriptVersion;
	uint32_t mInputDataVersion;
	uint8_t mTurboModInput[INPUT_BUFFER_SIZE];
	uint32_t mTurboModInputKeyCount;
} TConfigData;
static TConfigData sCfg;
static uint32_t sTurboModInputHeld = 0;
static uint32_t sTurboModInputJustHeld = 0;

static int32_t sLastTapInterval = TURBO_TAP_INTERVAL_DEFAULT;
static int32_t sLastAutoTap = AUTO_TAP_DEFAULT;
static int32_t sLastAutoHold = AUTO_HOLD_DEFAULT;

#define SHOW_MOD_TEXT_DURATION 2000.0
#define SHOW_MOD_TEXT_FADE_TIME 400.0
static char sShowModText[BIGP_CRT_MAX_LOCAL_BUFFER_SIZE];
static double sShowModTextTime = 0.0;

typedef struct
{
	uint64_t mDownFrame;
	uint32_t mTurboModeEnabled;
} TLocalButtonState;
static TLocalButtonState sButtonStates[MAX_INPUT_DEVICE_COUNT][MAX_EMU_BUTTON_COUNT];

static void show_current_turbo_mod_key()
{
	char keyString[BIGP_CRT_MAX_LOCAL_BUFFER_SIZE];
	char inputName[128];

	int strOfs = 0;
	
	if (!sCfg.mTurboModInputKeyCount)
	{
		return;
	}
	
	for (uint32_t keyIndex = 0; keyIndex < sCfg.mTurboModInputKeyCount; ++keyIndex)
	{
		if (bigpemu_input_get_input_name(inputName, sizeof(inputName), &sCfg.mTurboModInput[keyIndex * sInputSize]))
		{
			strOfs += sprintf(&keyString[strOfs], "%s%s", (keyIndex > 0) ? " + " : "", inputName);
		}
	}
	keyString[strOfs] = 0;
	
	assert(strOfs < (BIGP_CRT_MAX_LOCAL_BUFFER_SIZE - 1) && strOfs > 0);
	
	char locStr[BIGP_CRT_MAX_LOCAL_BUFFER_SIZE];
	strcpy(locStr, "STR_TURBOMODKEY");
	bigpemu_get_localized_string(locStr);

	sprintf(sShowModText, "%s %s", locStr, keyString);
	sShowModTextTime = bigpemu_time_ms();
}

static void render_center_text(const char *pTextBuffer, const float baseTextScale, const float xOffset, const float yOffset, const float alpha)
{
	//ui is in virtual 640x480 coordinates when using a 4:3 native display, but virtual coordinates will adjust according to the native aspect ratio
	float uiWH[2];
	bigpemu_drawui_get_virtual_canvas_size(uiWH);
	
	const float textScale = bigpemu_is_portrait_mode() ? baseTextScale : baseTextScale * 2.0f;
	
	const float wrapDist = (uiWH[0] - 12.0f) * (1.0f / textScale);
	float textRect[4]; //xMin, yMin, xMax, yMax
	if (bigpemu_drawui_text_bounds_ex(textRect, pTextBuffer, textScale, wrapDist))
	{
		const float textWidth = textRect[2] - textRect[0];
		const float textHeight = textRect[3] - textRect[1];
		
		const float rectEdgeDist = 4.0f;
		const float rectWidth = textWidth + rectEdgeDist * 2.0f;
		const float rectHeight = textHeight + rectEdgeDist * 2.0f;
		const float rectX = (uiWH[0] * 0.5f - rectWidth * 0.5f) + xOffset;
		const float rectY = (uiWH[1] * 0.5f - rectHeight * 0.5f) + yOffset;
		
		const float rectColorTop[4] = { 0.16f, 0.16f, 0.16f, 0.95f * alpha };
		const float rectColorBottom[4] = { 0.01f, 0.01f, 0.01f, 0.95f * alpha };
		const float rectColorBorder[4] = { 0.6f, 0.6f, 0.6f, 0.25f * alpha };
		//draw a rectangle which directly fits the text at whatever scale we've provided
		bigpemu_drawui_outlined_rect(rectX, rectY, rectWidth, rectHeight, rectColorTop, rectColorBottom, 1.0f, rectColorBorder);
		
		const float textColor[4] = { 1.0f, 1.0f, 1.0f, 1.0f * alpha };
		
		//subtracting the mins clamps the text to the rectangle edge - strings may otherwise start inset according to the natural font displacement
		bigpemu_drawui_text_ex(pTextBuffer, rectX + rectEdgeDist - textRect[0], rectY + rectEdgeDist - textRect[1], textScale, textColor, kDrawUI_TJ_Left, wrapDist);
	}
}

static uint32_t on_sw_loaded(const int eventHandle, void *pEventData)
{
	//reset the turbo flags each time new software is loaded
	memset(sButtonStates, 0, sizeof(sButtonStates));
}

static void update_user_ol_elem()
{
	if (sUserOLElemIndex >= 0)
	{
		//labels beginning with !STR_ will automatically perform a localized string lookup
		bigpemu_set_user_ol_elem_data(sUserOLElemIndex, "!STR_TURBO", sCfg.mTurboModInput, sCfg.mTurboModInputKeyCount, NULL);
	}
}

static uint32_t on_post_ui(const int eventHandle, void *pEventData)
{
	const uint32_t devCount = bigpemu_input_get_device_count();
	
	float dispYOffset = 0.0f;
	
	if (devCount)
	{
		if (sInModBindMode)
		{
			char locStr[BIGP_CRT_MAX_LOCAL_BUFFER_SIZE];
			strcpy(locStr, "STR_TURBOBINDPROMPT");
			bigpemu_get_localized_string(locStr);
		
			render_center_text(locStr, 1.0f, 0.0f, dispYOffset, 1.0f);
			dispYOffset += 36.0f;
		}
		
		if (sShowModTextTime)
		{
			const double curTime = bigpemu_time_ms();
			const double showDelta = curTime - sShowModTextTime;
			if (showDelta < SHOW_MOD_TEXT_DURATION)
			{
				const float alpha = (showDelta > (SHOW_MOD_TEXT_DURATION - SHOW_MOD_TEXT_FADE_TIME)) ?
				                     BIGPEMU_MIN((float)((SHOW_MOD_TEXT_DURATION - showDelta) / SHOW_MOD_TEXT_FADE_TIME), 1.0f) : 1.0f;
				render_center_text(sShowModText, 1.0f, 0.0f, dispYOffset, alpha);
				dispYOffset += 36.0f;
			}
			else
			{
				sShowModTextTime = 0.0;
			}
		}
	}
	
	//always defer this instead of showing it on startup, as loads may happen prior to menu/vm init (config load ordering)
	if (sWantInitialBindDisplay && devCount > 0 && sCfg.mTurboModInputKeyCount > 0)
	{
		show_current_turbo_mod_key();
		//translate it into a notify
		if (sShowModTextTime)
		{
			printf_notify("%s", sShowModText);
			sShowModTextTime = 0.0;
		}
		sWantInitialBindDisplay = 0;
	}
	
	const uint32_t turboModWasHeld = sTurboModInputHeld;
	sTurboModInputHeld = 0;
	
	const uint32_t heldCount = bigpemu_input_get_all_held_inputs(sInputBuffer, sMaxInputsInBuffer);

	if (sInModBindMode)
	{
		if (!devCount)
		{
			char locStr[BIGP_CRT_MAX_LOCAL_BUFFER_SIZE];
			strcpy(locStr, "STR_TURBONOGOOD");
			bigpemu_get_localized_string(locStr);
			printf_notify("%s", locStr);
			
			sInModBindMode = 0;
		}
		else if (bigpemu_menu_is_active())
		{
			switch (sModBindState)
			{
			case 1: //first, wait for no input to be held
				if (!heldCount)
				{
					++sModBindState;
				}
				break;
			case 2: //next, wait for input
				if (heldCount)
				{
					//add any buttons to the held set if they aren't already in it
					for (uint32_t heldIndex = 0; heldIndex < heldCount && sBindHeldCount < sMaxInputsInBuffer; ++heldIndex)
					{					
						if (!bigpemu_input_input_in_set(sBindInputBuffer, sBindHeldCount, &sInputBuffer[heldIndex * sInputSize]))
						{
							memcpy(&sBindInputBuffer[sBindHeldCount * sInputSize], &sInputBuffer[heldIndex * sInputSize], sInputSize);
							++sBindHeldCount;
						}
					}
				}
				else if (sBindHeldCount > 0)
				{
					memcpy(sCfg.mTurboModInput, sBindInputBuffer, sInputSize * sBindHeldCount);
					sCfg.mTurboModInputKeyCount = sBindHeldCount;
					
					bigpemu_set_cfg_data_blob(&sCfg, sizeof(sCfg), TURBO_CONFIG_BLOB_NAME);
				
					update_user_ol_elem();
				
					sInModBindMode = 0;
					show_current_turbo_mod_key();
				}
				break;
			default:
				assert(0); //shouldn't be possible
				sInModBindMode = 0;
				break;
			}
		}
		else
		{
			//if they backed out of the menu without setting a key, abort the process
			sInModBindMode = 0;
		}
	}
	else if (heldCount > 0 && sCfg.mTurboModInputKeyCount > 0)
	{
		sTurboModInputHeld = 1;
		for (uint32_t keyIndex = 0; keyIndex < sCfg.mTurboModInputKeyCount; ++keyIndex)
		{
			if (!bigpemu_input_input_in_set(sInputBuffer, heldCount, &sCfg.mTurboModInput[keyIndex * sInputSize]))
			{
				sTurboModInputHeld = 0;
				break;
			}
		}
	}
	
	if (!turboModWasHeld && sTurboModInputHeld)
	{
		sTurboModInputJustHeld = 1;
	}

	return 0;
}

static uint32_t on_video_frame(const int32_t eventHandle, void *pEventData)
{	
	bigpemu_get_setting_value(&sLastTapInterval, sTapIntervalHandle);
	bigpemu_get_setting_value(&sLastAutoTap, sAutoTapHandle);
	bigpemu_get_setting_value(&sLastAutoHold, sAutoHoldHandle);

	int32_t setNewMod = 0;
	bigpemu_get_setting_value(&setNewMod, sSetTurboModHandle);
	if (setNewMod)
	{
		//reset the bool as soon as we see it's been set, then flip us into bind mode
		setNewMod = 0;
		bigpemu_set_setting_value(&setNewMod, sSetTurboModHandle);
		
		if (!sInModBindMode)
		{
			sInModBindMode = 1;
			sModBindState = 1;
			sBindHeldCount = 0;
		}
	}
	
	return 0;
}

static void set_turbo_button_state(TBigPEmuInput *pInput, const TLocalButtonState *pBs, const uint32_t buttonBit, const uint64_t emuFrame, const uint64_t ivl)
{
	const uint64_t frameMod = (emuFrame - pBs->mDownFrame) % ivl;
	if (frameMod < (ivl >> 1) || sLastAutoHold)
	{
		pInput->mButtons |= buttonBit;
	}
	else
	{
		pInput->mButtons &= ~buttonBit;
	}
}

static uint32_t on_input_frame(const int32_t eventHandle, void *pEventData)
{
#ifndef _BIGP_PLATFORM_NEW_
	const uint64_t emuFrame = bigpemu_jag_get_frame_count();
#else
	const uint64_t emuFrame = bigpemu_emu_get_frame_count();
#endif
	const uint64_t ivl = (uint64_t)(sLastTapInterval + 1);

	TBigPEmuInputFrameParams *pParams = (TBigPEmuInputFrameParams *)pEventData;
	const uint32_t inputCount = BIGPEMU_MIN(pParams->mInputCount, MAX_INPUT_DEVICE_COUNT);
	for (uint32_t inputIndex = 0; inputIndex < inputCount; ++inputIndex)
	{
		TBigPEmuInput *pInput = pParams->mpInputs + inputIndex;
		for (uint32_t buttonIndex = 0; buttonIndex < MAX_EMU_BUTTON_COUNT; ++buttonIndex)
		{
			TLocalButtonState *pBs = &sButtonStates[inputIndex][buttonIndex];
			const uint32_t buttonBit = (1 << buttonIndex);
			if (pInput->mButtons & buttonBit)
			{
				if (!pBs->mDownFrame || sTurboModInputJustHeld)
				{
					pBs->mDownFrame = emuFrame;
					//if the turbo modifier is held, toggle turbo for this button on first-down
					if (sTurboModInputHeld)
					{
						pBs->mTurboModeEnabled ^= 1;
						char locStr[BIGP_CRT_MAX_LOCAL_BUFFER_SIZE];
						strcpy(locStr, (pBs->mTurboModeEnabled ? "STR_TURBOKEY_ENABLED" : "STR_TURBOKEY_DISABLED"));
						int locLen = bigpemu_get_localized_string(locStr);
						if (locLen >= 0)
						{
							locStr[locLen++] = ' ';
							const int btnLen = bigpemu_get_localized_emu_button_name(&locStr[locLen], buttonIndex);
							if (btnLen >= 0)
							{
								printf_notify("%s", locStr);
							}
						}
					}
				}
				
				if (pBs->mTurboModeEnabled)
				{
					set_turbo_button_state(pInput, pBs, buttonBit, emuFrame, ivl);
				}
			}
			else
			{
				pBs->mDownFrame = 0;
				if ((sLastAutoTap || sLastAutoHold) && pBs->mTurboModeEnabled)
				{
					set_turbo_button_state(pInput, pBs, buttonBit, emuFrame, ivl);
				}
			}
		}
	}
	
	//we reset this here because we might actually be running on the emulator thread, while the code setting this flag is on main.
	//(but not at the same time, so we don't have to worry about atomic access)
	sTurboModInputJustHeld = 0;
	
	return 0;
}

void bigp_init()
{
	void *pMod = bigpemu_get_module_handle();
	
	const int catHandle = bigpemu_register_setting_category(pMod, "Turbo Joy");
	sTapIntervalHandle = bigpemu_register_setting_int_full(catHandle, "Tap Interval", TURBO_TAP_INTERVAL_DEFAULT, 1, 240, 1);	
	sAutoTapHandle = bigpemu_register_setting_bool(catHandle, "Auto Tap", AUTO_TAP_DEFAULT);
	sAutoHoldHandle = bigpemu_register_setting_bool(catHandle, "Auto Hold", AUTO_HOLD_DEFAULT);
	sSetTurboModHandle = bigpemu_register_setting_bool(catHandle, "Set New Modifier", 0);

	sOnLoadEvent = bigpemu_register_event_sw_loaded(pMod, on_sw_loaded);
	sOnPostUIEvent = bigpemu_register_event_post_ui(pMod, on_post_ui);
	sOnVideoFrameEvent = bigpemu_register_event_video_thread_frame(pMod, on_video_frame);
	sOnInputFrameEvent = bigpemu_register_event_input_frame(pMod, on_input_frame);
		
	sInputSize = bigpemu_input_get_input_size();
	sMaxInputsInBuffer = BIGPEMU_MIN(INPUT_BUFFER_SIZE / sInputSize, 8);
	
	memset(sButtonStates, 0, sizeof(sButtonStates));
	
	const uint32_t inputDataVersion = bigpemu_input_get_input_data_version();
	
	uint32_t cfgDataSize;
	if (!bigpemu_get_cfg_data_blob(&sCfg, &cfgDataSize, sizeof(sCfg), TURBO_CONFIG_BLOB_NAME) || sCfg.mScriptVersion != TURBO_SCRIPT_VERSION ||
	    sCfg.mInputDataVersion != inputDataVersion || !sCfg.mTurboModInputKeyCount)
	{
		memset(&sCfg, 0, sizeof(sCfg));
		sCfg.mScriptVersion = TURBO_SCRIPT_VERSION;
		sCfg.mInputDataVersion = inputDataVersion;
		sCfg.mTurboModInputKeyCount = (bigpemu_input_create_input_from_vk(&sCfg.mTurboModInput[0 * sInputSize], BIGPEMU_VK_CONTROL) &&
									   bigpemu_input_create_input_from_vk(&sCfg.mTurboModInput[1 * sInputSize], 'T')) ? 2 : 0;
	}
	
	sUserOLElemIndex = bigpemu_claim_user_ol_elem();
	update_user_ol_elem();
	
	sWantInitialBindDisplay = 1;
	
	sInModBindMode = 0;
	sModBindState = 0;
	sBindHeldCount = 0;
	
	sTurboModInputHeld = 0;
	sTurboModInputJustHeld = 0;
	
	sShowModTextTime = 0.0;	
}

void bigp_shutdown()
{
	void *pMod = bigpemu_get_module_handle();
	bigpemu_unregister_event(pMod, sOnInputFrameEvent);
	bigpemu_unregister_event(pMod, sOnVideoFrameEvent);
	bigpemu_unregister_event(pMod, sOnPostUIEvent);
	bigpemu_unregister_event(pMod, sOnLoadEvent);
	sOnInputFrameEvent = -1;
	sOnVideoFrameEvent = -1;
	sOnPostUIEvent = -1;
	sOnLoadEvent = -1;
	sTapIntervalHandle = -1;
	sAutoTapHandle = -1;
	sAutoHoldHandle = -1;
	sSetTurboModHandle = -1;
	sUserOLElemIndex = -1;
	sLastTapInterval = TURBO_TAP_INTERVAL_DEFAULT;
	sLastAutoTap = AUTO_TAP_DEFAULT;
	sLastAutoHold = AUTO_HOLD_DEFAULT;
}
