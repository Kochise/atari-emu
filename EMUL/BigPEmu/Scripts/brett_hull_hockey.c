//__BIGPEMU_SCRIPT_MODULE__
//__BIGPEMU_META_DESC__		"Synchronization/flickering fix for Brett Hull Hockey."
//__BIGPEMU_META_AUTHOR__	"Rich Whitehouse"

#include "bigpcrt.h"
#include "jagregs.h"

static int sOnLoadEvent = -1;
static int sOnVideoEvent = -1;

static int sSyncFixSettingHandle = -1;

static int sLastSyncFix = 1;

#define BHULL_SUPPORTED_HASH					0xDAB00F552D115310ULL
#define BHULL_GPU_UPLOAD_PC						0x000056A4 //unused, just here for reference (a0 is program address)
#define BHULL_GPU_FIELD_RENDER_ADDR				0x25330 //also just for reference
#define BHULL_STALL_POINT_PC					0x0003C458
#define BHULL_MIN_VCOUNT_FOR_STALL				20

static void bh_stall_point_bp(const uint32_t addr)
{
	if (sLastSyncFix)
	{
		const int curVc = bigpemu_jag_read16(VC);
		if (curVc >= BHULL_MIN_VCOUNT_FOR_STALL)
		{
			//our timing hack is to just stall right before we wrap around, until we're near the start of scanout.
			//this ensures the blits from the GPU don't overlap a period where the OP is rendering from the destination buffer.
			bigpemu_jag_m68k_set_pc(BHULL_STALL_POINT_PC - 6);
			bigpemu_jag_m68k_consume_cycles(-1); //this eats the entire timeslice
			
			//another lazy hack, this game also has some timing issues due to not correctly synchronizing with gpu decompression.
			//we can fudge around this by just disabling pipeline emulation on the gpu. it's a terrible fix, but that's what scripts are for!
			bigpemu_jag_gpu_set_pipeline_enabled(0);
		}
	}
}

static uint32_t on_sw_loaded(const int eventHandle, void *pEventData)
{
	const uint64_t hash = bigpemu_get_loaded_fnv1a64();
	if (hash == BHULL_SUPPORTED_HASH)
	{
		bigpemu_jag_m68k_bp_add(BHULL_STALL_POINT_PC, bh_stall_point_bp);
	}
	return 0;
}

static uint32_t on_video_frame(const int eventHandle, void *pEventData)
{
	bigpemu_get_setting_value(&sLastSyncFix, sSyncFixSettingHandle);
	return 0;
}

void bigp_init()
{
	void *pMod = bigpemu_get_module_handle();
	
	bigpemu_set_module_usage_flags(pMod, BIGPEMU_MODUSAGE_DETERMINISMWARNING);
	
	const int catHandle = bigpemu_register_setting_category(pMod, "Brett Hull Hockey");
	sSyncFixSettingHandle = bigpemu_register_setting_bool(catHandle, "Sync Fix", 1);
	
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
	sSyncFixSettingHandle = -1;
	sLastSyncFix = 1;
}
