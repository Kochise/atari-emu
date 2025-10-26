//__BIGPEMU_SCRIPT_MODULE__
//__BIGPEMU_META_DESC__		"Optional throttling, music, etc. for DOOM."
//__BIGPEMU_META_AUTHOR__	"Rich Whitehouse"

#include "bigpcrt.h"
#include "jagregs.h"

static int32_t sOnLoadEvent = -1;
static int32_t sOnVideoEvent = -1;
static int32_t sOnStateSaveEvent = -1;
static int32_t sOnStateLoadEvent = -1;

static int sThrottleCountSettingHandle = -1;
static int sMusicSettingHandle = -1;

#define DOOM_DEFAULT_THROTTLE_COUNT			2
#define DOOM_DEFAULT_MUSIC					0

#define DOOM_PERS_DATA_VERSION				666

#define DOOM_SUPPORTED_HASH					0x75F2216BD4F54AC2ULL
#define DOOM_MENU_STALL_PC					0x00009CAA
#define DOOM_PSTART_PC						0x00013308
#define DOOM_STARTSONG_PC					0x00020390
#define DOOM_STOPSONG_PC					0x00020410
#define DOOM_PRERUNMUSIC_PC					0x00020302
#define DOOM_RANMUSIC_PC					0x00020366
#define DOOM_PLAYINTER_PC					0x00007118
#define DOOM_MINISTOP_PC					0x00009CB8
#define DOOM_DISPLAYLIST_ADDR				0x00047DBC
#define DOOM_TICCOUNT_ADDR					0x00047DA4
#define DOOM_PATCH_GPUPROG_ADDR				0x0001FF32
#define DOOM_GAMEMAP_ADDR					0x000407C4
#define DOOM_MUSIC_ADDR						0x0005515C
#define DOOM_DSPCODESTART_ADDR				0x00F1B030
#define DOOM_DSPFINISHED_ADDR				0x00F1B034
#define DOOM_SFXSTART_ADDR					0x00020EA4
#define DOOM_SAMPLECOUNT_ADDR				0x00F1B02C
#define DOOM_SAMPLEBUFFER_ADDR				0x001F0000
#define DOOM_SAMPLEBUFFER_SIZE				8192
#define DOOM_SAMPLEBUFFER_MASK				(DOOM_SAMPLEBUFFER_SIZE - 1)
#define DOOM_MUSICTIME_ADDR					0x0004D7F0
#define DOOM_SFXTIME_ADDR					0x0004DC14
#define DOOM_ENDLEVEL_MUSICID				7

//this allows us to track our own states without breaking save/load determinism
typedef struct
{
	uint32_t mVersion;
	uint32_t mInStall;
	uint32_t mTicStall;
	uint32_t mSaveSp;
	uint32_t mWaitOnSfx;
	uint32_t mPreSfxTime;
	uint8_t mMixData[DOOM_SAMPLEBUFFER_SIZE];
	uint8_t mPreservedBuffer[DOOM_SAMPLEBUFFER_SIZE];
	uint32_t mMixDataSize;
} TPersData;

static TPersData sPD;

static uint8_t sScratchBuffer[DOOM_SAMPLEBUFFER_SIZE];

static int sLastThrottleCount = DOOM_DEFAULT_THROTTLE_COUNT;
static int sLastMusic = DOOM_DEFAULT_MUSIC;

#define DOOM_MINI_LOOP_CHECK_PC				0x00009B7C
static const uint8_t skMiniLoopCheck[] =
{
	0x4E, 0x94, 0x26, 0x00, 0x4A, 0xB9, 0x00, 0x04, 0x09, 0x2C, 0x66, 0x08, 0x4A, 0xB9, 0x00, 0x04,
	0x08, 0xCC, 0x67, 0x04, 0x78, 0x04, 0x60, 0x14
};

static void doom_reset_pers_data()
{
	memset(&sPD, 0, sizeof(sPD));
	sPD.mVersion = DOOM_PERS_DATA_VERSION;
}

//doom's menu is an interesting case where the blitter and op are absolutely suffocating the cpu, in large part because of a background image that just about
//matches NTSC vertical resolution with the op being set to run on every line. the right thing to do would be to emulate bus priorities/contention, but we're
//not doing that because it's much more expensive! although there are options in the emulator to help the timing out here, the simple game-hack fix is to just
//recognize this case and manually stall the cpu. for the in-game case, you'll get a closer-to-hardware result by just slowing the blitter down, but making
//the blitter free and allowing the user to pick their frame cap is a happy medium.
static void doom_menu_stall_bp(const uint32_t addr)
{
	if (!bigpemu_jag_sysmemcmp(skMiniLoopCheck, DOOM_MINI_LOOP_CHECK_PC, sizeof(skMiniLoopCheck)))
	{
		//continuously update the gpu op from the throttle count (so that it auto-corrects if we disabled the option)
		const uint32_t writeThrottle = (sLastThrottleCount) ? (sLastThrottleCount + 1) : 2;
		const uint32_t newOp = 0x8C01 | (writeThrottle << 5);
		bigpemu_jag_write16(DOOM_PATCH_GPUPROG_ADDR, newOp);
	
		//perform a manual throttle in-place if we have a big old bus-eater up
		if (sLastThrottleCount)
		{
			const uint32_t olAddr = bigpemu_jag_read32(DOOM_DISPLAYLIST_ADDR);
			if (olAddr)
			{
				const uint64_t obj = bigpemu_jag_read64(olAddr + 16);
				const uint32_t bmHeight = (uint32_t)((obj >> 14ULL) & 1023ULL);
				if ((obj & 3ULL) == 0 && bmHeight >= 200)
				{
					const uint32_t ticCount = bigpemu_jag_read32(DOOM_TICCOUNT_ADDR);
					if (sPD.mInStall)
					{
						if (ticCount >= sPD.mTicStall)
						{
							sPD.mInStall = 0;
						}
					}
					else
					{
						//intentionally one-off from the gpu throttle
						sPD.mTicStall = ticCount + sLastThrottleCount;
						sPD.mInStall = 1;
					}
					
					if (sPD.mInStall)
					{
						bigpemu_jag_m68k_set_pc(DOOM_MENU_STALL_PC - 2);
						bigpemu_jag_m68k_consume_cycles(1024);
					}
				}
				else
				{
					sPD.mInStall = 0;
				}
			}
		}
	}
}

static void doom_pstart_bp(const uint32_t addr)
{
	if (sPD.mSaveSp)
	{
		bigpemu_jag_m68k_set_areg(7, sPD.mSaveSp);
		sPD.mSaveSp = 0;
		return;
	}
	
	if (sLastMusic)
	{
		//we only have a limited track list present in the retail image, so do the same nonsense the intermission code does
		const uint32_t gameMap = bigpemu_jag_read32(DOOM_GAMEMAP_ADDR);
		const uint32_t musicId = ((gameMap - 1) % 10) + 1;

		const uint32_t curSp = bigpemu_jag_m68k_get_areg(7);
		sPD.mSaveSp = curSp;
		const uint32_t newSp = curSp - 12;
		bigpemu_jag_m68k_set_areg(7, newSp);
		bigpemu_jag_write32(newSp, addr);
		bigpemu_jag_write32(newSp + 4, musicId);
		bigpemu_jag_write32(newSp + 8, 1); //looping
		bigpemu_jag_m68k_set_pc(DOOM_STARTSONG_PC);
	}
}

static void doom_prerunmusic_bp(const uint32_t addr)
{	
	if (sPD.mWaitOnSfx)
	{
		if (bigpemu_jag_read32(DOOM_DSPFINISHED_ADDR) == 0xDEF6)
		{
			sPD.mWaitOnSfx = 0;
				
			//copy the sfx data out
			const uint32_t sfxCount = bigpemu_jag_read32(DOOM_SFXTIME_ADDR) - sPD.mPreSfxTime;
			const uint32_t bytesStart = sPD.mPreSfxTime * 2;
			const uint32_t bytesEnd = (sPD.mPreSfxTime + sfxCount) * 2;
			const uint32_t bytesCount = BIGPEMU_MIN(bytesEnd - bytesStart, DOOM_SAMPLEBUFFER_SIZE);
			const uint32_t bufferOffset = (bytesStart & DOOM_SAMPLEBUFFER_MASK);
			const uint32_t bufferAddr = DOOM_SAMPLEBUFFER_ADDR + bufferOffset;
			const uint32_t firstChunkSize = BIGPEMU_MIN(bytesCount, DOOM_SAMPLEBUFFER_SIZE - bufferOffset);
			
			sPD.mMixDataSize = BIGPEMU_MIN(sfxCount * 2, DOOM_SAMPLEBUFFER_SIZE);
			bigpemu_jag_sysmemread(sPD.mMixData, bufferAddr, firstChunkSize);
			if (firstChunkSize < bytesCount)
			{
				const uint32_t remSize = bytesCount - firstChunkSize;
				bigpemu_jag_sysmemread(&sPD.mMixData[firstChunkSize], DOOM_SAMPLEBUFFER_ADDR, remSize);
			}
			//hack, just blast the whole preserved buffer back in place so we don't have to worry about music/sfx writing in
			//different windows
			bigpemu_jag_sysmemwrite(sPD.mPreservedBuffer, DOOM_SAMPLEBUFFER_ADDR, DOOM_SAMPLEBUFFER_SIZE);
		}
		else
		{
			bigpemu_jag_m68k_set_pc(DOOM_PRERUNMUSIC_PC - 6);
			bigpemu_jag_m68k_consume_cycles(1024);
		}
	}
	else if (sLastMusic)
	{
		bigpemu_jag_sysmemread(sPD.mPreservedBuffer, DOOM_SAMPLEBUFFER_ADDR, DOOM_SAMPLEBUFFER_SIZE);
	
		sPD.mPreSfxTime = bigpemu_jag_read32(DOOM_SFXTIME_ADDR);		
		bigpemu_jag_write32(DOOM_DSPFINISHED_ADDR, 0x1234);
		bigpemu_jag_write32(DOOM_DSPCODESTART_ADDR, DOOM_SFXSTART_ADDR);
		bigpemu_jag_m68k_set_pc(DOOM_PRERUNMUSIC_PC - 6);
		sPD.mWaitOnSfx = 1;
	}
}

static void doom_ranmusic_bp(const uint32_t addr)
{
	if (sLastMusic)
	{
		//now that both music and sfx are done, mix them back into the doom buffer
		const uint32_t bytesStart = sPD.mPreSfxTime * 2;
		const uint32_t bytesCount = sPD.mMixDataSize;
		const uint32_t bufferOffset = (bytesStart & DOOM_SAMPLEBUFFER_MASK);
		const uint32_t bufferAddr = DOOM_SAMPLEBUFFER_ADDR + bufferOffset;
		const uint32_t firstChunkSize = BIGPEMU_MIN(bytesCount, DOOM_SAMPLEBUFFER_SIZE - bufferOffset);
		bigpemu_jag_sysmemread(sScratchBuffer, bufferAddr, firstChunkSize);
		bigpemu_audio_mix_int16((int16_t *)sPD.mMixData, (const int16_t *)&sScratchBuffer, firstChunkSize >> 1, 1);
		bigpemu_jag_sysmemwrite(sPD.mMixData, bufferAddr, firstChunkSize);
		if (firstChunkSize < bytesCount)
		{
			const uint32_t remSize = bytesCount - firstChunkSize;
			bigpemu_jag_sysmemread(&sScratchBuffer[firstChunkSize], DOOM_SAMPLEBUFFER_ADDR, remSize);
			bigpemu_audio_mix_int16((int16_t *)&sPD.mMixData[firstChunkSize], (const int16_t *)&sScratchBuffer[firstChunkSize], remSize >> 1, 1);
			bigpemu_jag_sysmemwrite(&sPD.mMixData[firstChunkSize], DOOM_SAMPLEBUFFER_ADDR, remSize);
		}
	}
}

static void doom_playinter_bp(const uint32_t addr)
{
	if (sLastMusic)
	{
		//play the actual intermission music
		const uint32_t curSp = bigpemu_jag_m68k_get_areg(7);
		const uint32_t newSp = curSp - 4;
		bigpemu_jag_m68k_set_areg(7, newSp);
		bigpemu_jag_write32(newSp, DOOM_ENDLEVEL_MUSICID);
		bigpemu_jag_m68k_set_pc(DOOM_PLAYINTER_PC + 6);		
	}
}

static void doom_ministop_bp(const uint32_t addr)
{
	if (sPD.mSaveSp)
	{
		bigpemu_jag_m68k_set_areg(7, sPD.mSaveSp);
		sPD.mSaveSp = 0;
		return;
	}
	
	if (sLastMusic)
	{
		//make sure music is stopped when exiting any miniloop
		const uint32_t musicPtr = bigpemu_jag_read32(DOOM_MUSIC_ADDR);
		if (musicPtr)
		{
			const uint32_t curSp = bigpemu_jag_m68k_get_areg(7);
			sPD.mSaveSp = curSp;
			const uint32_t newSp = curSp - 4;
			bigpemu_jag_m68k_set_areg(7, newSp);
			bigpemu_jag_write32(newSp, addr);
			bigpemu_jag_m68k_set_pc(DOOM_STOPSONG_PC);
		}
	}
}

static void doom_stopsafety_bp(const uint32_t addr)
{
	//prevent attempts to double-free music
	const uint32_t musicPtr = bigpemu_jag_read32(DOOM_MUSIC_ADDR);
	if (!musicPtr)
	{
		bigpemu_jag_m68k_set_pc(DOOM_STOPSONG_PC + 64);
	}	
}

static uint32_t on_sw_loaded(const int eventHandle, void *pEventData)
{
	const uint64_t hash = bigpemu_get_loaded_fnv1a64();
	if (hash == DOOM_SUPPORTED_HASH)
	{
		doom_reset_pers_data();
		bigpemu_jag_blitter_set_excycles(0); //just make the blitter free to ensure consistent timing, since we're manually stalling
		bigpemu_jag_m68k_bp_add(DOOM_MENU_STALL_PC, doom_menu_stall_bp);
		bigpemu_jag_m68k_bp_add(DOOM_PSTART_PC, doom_pstart_bp);
		bigpemu_jag_m68k_bp_add(DOOM_PRERUNMUSIC_PC, doom_prerunmusic_bp);
		bigpemu_jag_m68k_bp_add(DOOM_RANMUSIC_PC, doom_ranmusic_bp);
		bigpemu_jag_m68k_bp_add(DOOM_PLAYINTER_PC, doom_playinter_bp);
		bigpemu_jag_m68k_bp_add(DOOM_MINISTOP_PC, doom_ministop_bp);
		bigpemu_jag_m68k_bp_add(DOOM_STOPSONG_PC, doom_stopsafety_bp);
	}
	return 0;
}

static uint32_t on_video_frame(const int eventHandle, void *pEventData)
{
	bigpemu_get_setting_value(&sLastThrottleCount, sThrottleCountSettingHandle);
	bigpemu_get_setting_value(&sLastMusic, sMusicSettingHandle);
	return 0;
}

static uint32_t on_save_state(const int32_t eventHandle, void *pEventData)
{
	memcpy(pEventData, &sPD, sizeof(sPD));
	return sizeof(sPD);
}

static uint32_t on_load_state(const int32_t eventHandle, void *pEventData)
{
	if (*(int32_t *)pEventData == DOOM_PERS_DATA_VERSION)
	{
		memcpy(&sPD, pEventData, sizeof(sPD));
	}
	return 0;
}

void bigp_init()
{
	void *pMod = bigpemu_get_module_handle();
	
	bigpemu_set_module_usage_flags(pMod, BIGPEMU_MODUSAGE_DETERMINISMWARNING);
	
	const int catHandle = bigpemu_register_setting_category(pMod, "DOOM");
	sThrottleCountSettingHandle = bigpemu_register_setting_int_full(catHandle, "Frame throttle", DOOM_DEFAULT_THROTTLE_COUNT, 0, 5, 1);
	sMusicSettingHandle = bigpemu_register_setting_bool(catHandle, "Music", DOOM_DEFAULT_MUSIC);
	
	sOnLoadEvent = bigpemu_register_event_sw_loaded(pMod, on_sw_loaded);
	sOnVideoEvent = bigpemu_register_event_video_thread_frame(pMod, on_video_frame);
	sOnStateSaveEvent = bigpemu_register_event_save_state(pMod, on_save_state, DOOM_SUPPORTED_HASH);
	sOnStateLoadEvent = bigpemu_register_event_load_state(pMod, on_load_state, DOOM_SUPPORTED_HASH);
	
	doom_reset_pers_data();
}

void bigp_shutdown()
{
	void *pMod = bigpemu_get_module_handle();
	bigpemu_unregister_event(pMod, sOnLoadEvent);
	bigpemu_unregister_event(pMod, sOnVideoEvent);
	bigpemu_unregister_event(pMod, sOnStateSaveEvent);
	bigpemu_unregister_event(pMod, sOnStateLoadEvent);
	sOnLoadEvent = -1;
	sOnVideoEvent = -1;
	sOnStateSaveEvent = -1;
	sOnStateLoadEvent = -1;
	
	sThrottleCountSettingHandle = -1;
	sLastThrottleCount = DOOM_DEFAULT_THROTTLE_COUNT;
	sMusicSettingHandle = -1;
	sLastMusic = DOOM_DEFAULT_MUSIC;
}
