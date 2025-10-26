//__BIGPEMU_SCRIPT_MODULE__
//__BIGPEMU_META_DESC__		"Demonstrates cycling through various aspect ratio and display area combinations."
//__BIGPEMU_META_AUTHOR__	"Rich Whitehouse"

#include "bigpcrt.h"
#include "jagregs.h"

static int sOnVideoFrameEvent = -1;
static int sOnEmuFrameEvent = -1;

static int32_t sRatioModeHandle = -1;
static int32_t sLastRatioMode = -1;

typedef struct
{
	int32_t mLX;
	int32_t mRX;
	int32_t mTY;
	int32_t mBY;
	double mRatio;
} SRatioAdjustMode;

//although this will also work in pal mode, we're ignoring the standard pal window.
//if we cared about pal, we'd set up another table to use when !bigpemu_jag_is_ntsc().
static const SRatioAdjustMode skRatioModes[] =
{
	{ 176, 1456, 14, 254, 320.0 / 240.0 }, //regular 4:3 with some reasonable offsets
	{ 106, 1526, 30, 230, 1920.0 / 1080.0 }, //"zoomed" 16:9, expanding horizontal window and crushing to 200 vertical lines
	{ 0, 1690, 14, 252, 1920.0 / 1080.0 } //16:9 using the entire horizontal window with 238 vertical lines
};
static const uint32_t skRatioModeCount = BIGPEMU_ARRAY_LENGTH(skRatioModes);

static uint32_t on_video_frame(const int32_t eventHandle, void *pEventData)
{
	bigpemu_get_setting_value(&sLastRatioMode, sRatioModeHandle);
	return 0;
}

static uint32_t on_emu_frame(const int32_t eventHandle, void *pEventData)
{
	if (sLastRatioMode < 0)
	{
		bigpemu_jag_force_display_bounds(NULL);
		bigpemu_jag_force_display_ratio(0.0);
	}
	else
	{
		const SRatioAdjustMode *pMode = &skRatioModes[sLastRatioMode];
		bigpemu_jag_force_display_bounds(&pMode->mLX);
		bigpemu_jag_force_display_ratio(pMode->mRatio);
	}
	return 0;
}

void bigp_init()
{
	void *pMod = bigpemu_get_module_handle();
	
	const int catHandle = bigpemu_register_setting_category(pMod, "Ratio Adjust");
	sRatioModeHandle = bigpemu_register_setting_int_full(catHandle, "Mode", -1, -1, skRatioModeCount - 1, 1);	

	sOnEmuFrameEvent = bigpemu_register_event_emu_thread_frame(pMod, on_emu_frame);
	sOnVideoFrameEvent = bigpemu_register_event_video_thread_frame(pMod, on_video_frame);
}

void bigp_shutdown()
{
	void *pMod = bigpemu_get_module_handle();
	bigpemu_unregister_event(pMod, sOnVideoFrameEvent);
	bigpemu_unregister_event(pMod, sOnEmuFrameEvent);
	sOnVideoFrameEvent = -1;
	sOnEmuFrameEvent = -1;
	sRatioModeHandle = -1;
	sLastRatioMode = -1;
}
