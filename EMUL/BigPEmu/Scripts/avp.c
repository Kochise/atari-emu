//__BIGPEMU_SCRIPT_MODULE__
//__BIGPEMU_META_DESC__		"Provides additional functionality for AvP."
//__BIGPEMU_META_AUTHOR__	"Rich Whitehouse"

#include "bigpcrt.h"

static int sOnLoadEvent = -1;
static int sOnVideoEvent = -1;

static int sVBCountSettingHandle = -1;
static int sEnableMouseLookHandle = -1;
static int sMouseLookSensHandle = -1;

#define AVP_DEFAULT_VBCOUNT			4
#define AVP_DEFAULT_MOUSELOOK		1
#define AVP_DEFAULT_MOUSESENS		16

//normally 4 on PAL (16Hz), 5 on NTSC (15Hz)
//a default of 4 under NTSC will mean 20Hz.
static int sLastVBCount = AVP_DEFAULT_VBCOUNT;

static int sLastMouseLook = AVP_DEFAULT_MOUSELOOK;
static int sLastMouseLookSens = AVP_DEFAULT_MOUSESENS;

//feel free to add more rom variants, but memory locations may also need to change
#define AVP_SUPPORTED_HASH					0x1BCB3230DB41AA47ULL
#define AVP_FINISH_ANGLES_PC				0xFF04
#define AVP_YAW_ANGLE_ADDR					0x2F1D6
#define AVP_JUST_SET_PARAMS_PC				0x9094 //common point between PAL and NTSC
#define AVP_RAM_CODE_COMPARE_ADDR			0x9022
#define AVP_VBCOUNT_ADDR					0x2EEA0 //this is the word the game reads to determine how many vblanks to eat between game frames

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

//this is a breakpoint callback function, so it'll be running on the emulator thread.
//this makes all bigpemu_jag_* functions safe to call without synchronization.
static void avp_just_set_vid_params_bp(const uint32_t addr)
{
	//when keeping a breakpoint in any kind of mutable (e.g. ram) memory, it's a good idea to make sure the code we're breaking on.
	//is the code we expect to be present. this isn't really necessary in avp's case, but i'm doing it here to establish good practice for anyone using this as an example!
	if (!bigpemu_jag_sysmemcmp(skAvpExpectedRamCode, AVP_RAM_CODE_COMPARE_ADDR, sizeof(skAvpExpectedRamCode)))
	{
		//the (uncapped by hardware performance) framerate is:
		//NTSC - 60 / (vbcount - 1)
		//PAL - 50 / (vbcount - 1)
		printf("AvP: Overwriting vbcount '%i' with '%i'.\n", (int)bigpemu_jag_read16(AVP_VBCOUNT_ADDR), sLastVBCount);
		//looks legit, now let's stomp the vb count with our own setting
		bigpemu_jag_write16(AVP_VBCOUNT_ADDR, (uint16_t)sLastVBCount);
		//note that an alternate approach here would be to simply stomp the word at AVP_VBCOUNT_ADDR with our value every frame via the emulator frame event, which occurs
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
				uint8_t angle = bigpemu_jag_read8(AVP_YAW_ANGLE_ADDR);
				const float analYaw = anal[0];
				if (analYaw)
				{
					const float fSign = (analYaw < 0.0f) ? -1.0f : 1.0f;
					angle -= BIGPEMU_MAX(fabsf(analYaw) * (float)sLastMouseLookSens, 1.0f) * fSign;
					bigpemu_jag_write8(AVP_YAW_ANGLE_ADDR, angle);
				}
				break;
			}
		}
	}
}

static uint32_t on_sw_loaded(const int eventHandle, void *pEventData)
{
	const uint64_t hash = bigpemu_get_loaded_fnv1a64();
	if (hash == AVP_SUPPORTED_HASH)
	{
		//this is the game we're after, so set a breakpoint just after the game has initialized video parameters
		bigpemu_jag_m68k_bp_add(AVP_JUST_SET_PARAMS_PC, avp_just_set_vid_params_bp);
		bigpemu_jag_m68k_bp_add(AVP_FINISH_ANGLES_PC, avp_finish_angles_bp);
	}
	return 0;
}

static uint32_t on_video_frame(const int eventHandle, void *pEventData)
{
	bigpemu_get_setting_value(&sLastVBCount, sVBCountSettingHandle);
	bigpemu_get_setting_value(&sLastMouseLook, sEnableMouseLookHandle);
	bigpemu_get_setting_value(&sLastMouseLookSens, sMouseLookSensHandle);
	return 0;
}

void bigp_init()
{
	void *pMod = bigpemu_get_module_handle();
	
	const int catHandle = bigpemu_register_setting_category(pMod, "AvP");
	sVBCountSettingHandle = bigpemu_register_setting_int_full(catHandle, "VB Count", AVP_DEFAULT_VBCOUNT, 1, 6, 1);
	sEnableMouseLookHandle = bigpemu_register_setting_bool(catHandle, "Mouselook", AVP_DEFAULT_MOUSELOOK);
	sMouseLookSensHandle = bigpemu_register_setting_int_full(catHandle, "Sensitivity", AVP_DEFAULT_MOUSESENS, 1, 128, 1);
	
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
	sLastVBCount = AVP_DEFAULT_VBCOUNT;
	sLastMouseLook = AVP_DEFAULT_MOUSELOOK;
	sLastMouseLookSens = AVP_DEFAULT_MOUSESENS;
}
