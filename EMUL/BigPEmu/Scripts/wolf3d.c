//__BIGPEMU_SCRIPT_MODULE__
//__BIGPEMU_META_DESC__		"Provides additional functionality for Wolf3D."
//__BIGPEMU_META_AUTHOR__	"Rich Whitehouse"

#include "bigpcrt.h"

static int sOnLoadEvent = -1;
static int sOnVideoEvent = -1;

static int sEnableMouseLookHandle = -1;
static int sMouseLookSensHandle = -1;
static int sAlwaysStrafeHandle = -1;
static int sAlwaysRunHandle = -1;

#define WOLF3D_DEFAULT_MOUSELOOK		1
#define WOLF3D_DEFAULT_MOUSESENS		16
#define WOLF3D_DEFAULT_ALWAYSSTRAFE		0
#define WOLF3D_DEFAULT_ALWAYSRUN		0
#define WOLF3D_ANALOG_BASE_SCALE		2.0f

static int sLastMouseLook = WOLF3D_DEFAULT_MOUSELOOK;
static int sLastMouseLookSens = WOLF3D_DEFAULT_MOUSESENS;
static int sLastAlwaysStrafe = WOLF3D_DEFAULT_ALWAYSSTRAFE;
static int sLastAlwaysRun = WOLF3D_DEFAULT_ALWAYSRUN;

#define WOLF3D_SUPPORTED_HASH						0xF714BD1B17D9375DULL
#define WOLF3D_ANGLE_ADDR							0x00023B04
#define WOLF3D_PCADDR_ADJUST_ANGLES					0x00015410
#define WOLF3D_PCADDR_CHECK_STRAFE					0x0001532A
#define WOLF3D_PCADDR_CHECK_RUN						0x000152D0

static void wolf3d_adjust_angles_bp(const uint32_t addr)
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
				const float analYaw = anal[0];
				if (analYaw)
				{
					uint32_t angle = bigpemu_jag_read32(WOLF3D_ANGLE_ADDR);
					const float fSign = (analYaw < 0.0f) ? -1.0f : 1.0f;
					angle -= BIGPEMU_MAX(fabsf(analYaw) * (float)sLastMouseLookSens * WOLF3D_ANALOG_BASE_SCALE, 1.0f) * fSign;
					bigpemu_jag_write32(WOLF3D_ANGLE_ADDR, angle & 0x7FF);
				}
				break;
			}
		}
	}
}

static void wolf3d_always_strafe_bp(const uint32_t addr)
{
	if (!sLastAlwaysStrafe)
	{
		return;
	}
	
	//this will skip the and/branch and always take the "strafe held" path
	bigpemu_jag_m68k_set_pc(addr + 8);
}

static void wolf3d_always_run_bp(const uint32_t addr)
{
	if (!sLastAlwaysRun)
	{
		return;
	}

	//code block functions just like strafe check
	bigpemu_jag_m68k_set_pc(addr + 8);
}

static uint32_t on_sw_loaded(const int eventHandle, void *pEventData)
{
	const uint64_t hash = bigpemu_get_loaded_fnv1a64();
	if (hash == WOLF3D_SUPPORTED_HASH)
	{
		bigpemu_jag_m68k_bp_add(WOLF3D_PCADDR_ADJUST_ANGLES, wolf3d_adjust_angles_bp);
		bigpemu_jag_m68k_bp_add(WOLF3D_PCADDR_CHECK_STRAFE, wolf3d_always_strafe_bp);
		bigpemu_jag_m68k_bp_add(WOLF3D_PCADDR_CHECK_RUN, wolf3d_always_run_bp);
	}
	return 0;
}

static uint32_t on_video_frame(const int eventHandle, void *pEventData)
{
	bigpemu_get_setting_value(&sLastMouseLook, sEnableMouseLookHandle);
	bigpemu_get_setting_value(&sLastMouseLookSens, sMouseLookSensHandle);
	bigpemu_get_setting_value(&sLastAlwaysStrafe, sAlwaysStrafeHandle);
	bigpemu_get_setting_value(&sLastAlwaysRun, sAlwaysRunHandle);
	return 0;
}

void bigp_init()
{
	void *pMod = bigpemu_get_module_handle();
	
	bigpemu_set_module_usage_flags(pMod, BIGPEMU_MODUSAGE_DETERMINISMWARNING);
	
	const int catHandle = bigpemu_register_setting_category(pMod, "Wolf3D");
	sEnableMouseLookHandle = bigpemu_register_setting_bool(catHandle, "Mouselook", WOLF3D_DEFAULT_MOUSELOOK);
	sMouseLookSensHandle = bigpemu_register_setting_int_full(catHandle, "Sensitivity", WOLF3D_DEFAULT_MOUSESENS, 1, 128, 1);
	sAlwaysStrafeHandle = bigpemu_register_setting_bool(catHandle, "Always Strafe", WOLF3D_DEFAULT_ALWAYSSTRAFE);
	sAlwaysRunHandle = bigpemu_register_setting_bool(catHandle, "Always Run", WOLF3D_DEFAULT_ALWAYSRUN);
	
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
	sEnableMouseLookHandle = -1;
	sMouseLookSensHandle = -1;
	sAlwaysStrafeHandle = -1;
	sAlwaysRunHandle = -1;
	sLastMouseLook = WOLF3D_DEFAULT_MOUSELOOK;
	sLastMouseLookSens = WOLF3D_DEFAULT_MOUSESENS;
	sLastAlwaysStrafe = WOLF3D_DEFAULT_ALWAYSSTRAFE;
	sLastAlwaysRun = WOLF3D_DEFAULT_ALWAYSRUN;
}
