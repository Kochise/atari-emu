//__BIGPEMU_SCRIPT_MODULE__
//__BIGPEMU_META_DESC__		"Provides additional functionality for Iron Soldier."
//__BIGPEMU_META_AUTHOR__	"Rich Whitehouse"

#include "bigpcrt.h"

static int sOnLoadEvent = -1;
static int sOnVideoEvent = -1;

static int sEnableMouseLookHandle = -1;
static int sMouseLookSensHandle = -1;
static int sMouseLookVSensHandle = -1;

#define IS_DEFAULT_MOUSELOOK		1
#define IS_DEFAULT_MOUSESENS		16
#define IS_DEFAULT_MOUSEVSENS		12
#define IS_ANALOG_BASE_SCALE		2.0f
#define IS_ANALOG_ACCEL_SCALE		131072.0f

static int sLastMouseLook = IS_DEFAULT_MOUSELOOK;
static int sLastMouseLookSens = IS_DEFAULT_MOUSESENS;
static int sLastMouseLookVSens = IS_DEFAULT_MOUSEVSENS;

#define IS_SUPPORTED_HASH						0x8429FDCFCDF35FA1ULL
#define IS_ANGLE_YAW_ADDR						0x000046D2
#define IS_ANGLE_PITCH_ADDR						0x000046DA
#define IS_ANGLE_PITCH_MIN						-252
#define IS_ANGLE_PITCH_MAX						508
#define IS_TORSO_FLAG_ADDR						0x00004644
#define IS_TORSO_YAW_ADDR						0x000046DE
#define IS_TORSO_YAW_MIN						-510
#define IS_TORSO_YAW_MAX						512
#define IS_ACCEL_ADDR							0x000046EC
#define IS_ACCEL_MIN							-393216
#define IS_ACCEL_MAX							1048576
#define IS_PCADDR_ADJUST_ANGLES					0x0000B522

static float analog_intensity_to_angle_offset(const float analIntensity, const int32_t sens)
{
	const float fSign = (analIntensity < 0.0f) ? -1.0f : 1.0f;
	return BIGPEMU_MAX(fabsf(analIntensity) * (float)sens * IS_ANALOG_BASE_SCALE, 1.0f) * fSign;
}

static void apply_offset_to_signed_angle(const uint32_t angleAddr, const int32_t angleMin, const int32_t angleMax, const float angleOffset)
{
	int32_t angle = ((int16_t)(bigpemu_jag_read16(angleAddr) << 5) >> 5);
	angle += angleOffset;
	angle = BIGPEMU_MAX(angle, angleMin);
	angle = BIGPEMU_MIN(angle, angleMax);
	bigpemu_jag_write16(angleAddr, (uint16_t)angle & 0x7FF);	
}

static void is_adjust_angles_bp(const uint32_t addr)
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
			const uint32_t analCount = bigpemu_jag_get_analogs(anal, inputIndex);
			if (analCount >= 2)
			{
				const float analYaw = anal[0];
				if (analYaw)
				{
					if (bigpemu_jag_read16(IS_TORSO_FLAG_ADDR) != 0xFFFF)
					{
						uint32_t angle = bigpemu_jag_read16(IS_ANGLE_YAW_ADDR);
						angle += analog_intensity_to_angle_offset(analYaw, sLastMouseLookSens);
						bigpemu_jag_write16(IS_ANGLE_YAW_ADDR, angle & 0x7FF);
					}
					else
					{
						//when angling the torso independently, control that angle instead
						apply_offset_to_signed_angle(IS_TORSO_YAW_ADDR, IS_TORSO_YAW_MIN, IS_TORSO_YAW_MAX, analog_intensity_to_angle_offset(analYaw, (float)sLastMouseLookSens));
					}
				}
				const float analPitch = anal[1];
				if (analPitch)
				{
					apply_offset_to_signed_angle(IS_ANGLE_PITCH_ADDR, IS_ANGLE_PITCH_MIN, IS_ANGLE_PITCH_MAX, analog_intensity_to_angle_offset(analPitch, (float)sLastMouseLookVSens));
				}
				//try applying acceleration as well
				if (analCount >= 4)
				{
					const float analAccel = -anal[3];
					if (analAccel)
					{
						int32_t accel = (int32_t)bigpemu_jag_read32(IS_ACCEL_ADDR);
						accel += analAccel * IS_ANALOG_ACCEL_SCALE;
						//this min/max is imposed by the game, but if you want to mess around, you can remove the clamps. nothing will break, and absurd move speeds can be achieved.
						accel = BIGPEMU_MAX(accel, IS_ACCEL_MIN);
						accel = BIGPEMU_MIN(accel, IS_ACCEL_MAX);
						bigpemu_jag_write32(IS_ACCEL_ADDR, (uint32_t)accel);
					}
				}
				break;
			}
		}
	}	
}

static uint32_t on_sw_loaded(const int eventHandle, void *pEventData)
{
	const uint64_t hash = bigpemu_get_loaded_fnv1a64();
	if (hash == IS_SUPPORTED_HASH)
	{
		bigpemu_jag_m68k_bp_add(IS_PCADDR_ADJUST_ANGLES, is_adjust_angles_bp);
	}
	return 0;
}

static uint32_t on_video_frame(const int eventHandle, void *pEventData)
{
	bigpemu_get_setting_value(&sLastMouseLook, sEnableMouseLookHandle);
	bigpemu_get_setting_value(&sLastMouseLookSens, sMouseLookSensHandle);
	bigpemu_get_setting_value(&sLastMouseLookVSens, sMouseLookVSensHandle);
	return 0;
}

void bigp_init()
{
	void *pMod = bigpemu_get_module_handle();
	
	bigpemu_set_module_usage_flags(pMod, BIGPEMU_MODUSAGE_DETERMINISMWARNING);
	
	const int catHandle = bigpemu_register_setting_category(pMod, "Iron Soldier");
	sEnableMouseLookHandle = bigpemu_register_setting_bool(catHandle, "Mouselook", IS_DEFAULT_MOUSELOOK);
	sMouseLookSensHandle = bigpemu_register_setting_int_full(catHandle, "H Sensitivity", IS_DEFAULT_MOUSESENS, 1, 128, 1);
	sMouseLookVSensHandle = bigpemu_register_setting_int_full(catHandle, "V Sensitivity", IS_DEFAULT_MOUSEVSENS, 1, 128, 1);
	
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
	sMouseLookVSensHandle = -1;
	sLastMouseLook = IS_DEFAULT_MOUSELOOK;
	sLastMouseLookSens = IS_DEFAULT_MOUSESENS;
	sLastMouseLookVSens = IS_DEFAULT_MOUSEVSENS;
}
