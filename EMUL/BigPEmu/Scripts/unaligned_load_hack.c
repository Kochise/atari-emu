//__BIGPEMU_SCRIPT_MODULE__
//__BIGPEMU_META_DESC__		"Unaligned load hack, primarily meant for earlier 3D Stooges titles."
//__BIGPEMU_META_AUTHOR__	"Rich Whitehouse"

#include "bigpcrt.h"
#include "jagregs.h"

static int sOnLoadEvent = -1;

typedef struct
{
	uint64_t mSwHash;
	uint32_t mManCpuBpAddr;
	uint32_t mRamPatchAddr;
} TSupportedGameEntry;

static const TSupportedGameEntry skSupportedGames[] =
{
	{ 0x627801118F5AB655ULL, 0x008023CC, 0x000C4630 }, //mad bodies
	{ 0x4B58EE4C3124B436ULL, 0x008023CC, 0x000C4630 }, //mad bodies (alt)
	{ 0xA9D50F45A2578F06ULL, 0x00802022, 0x00104320 }, //gorf classic
	{ 0xE01253057900AA57ULL, 0x00004000, 0x00104320 } //gorf classic (alt)
};
static const uint32_t skSupportedGameCount = BIGPEMU_ARRAY_LENGTH(skSupportedGames);

static uint32_t sGameIndex = 0;

static void man_cpu_bp(const uint32_t addr)
{
	const TSupportedGameEntry *pGame = &skSupportedGames[sGameIndex];
	//these titles rely on some very obscure unaligned load behavior, which is handled correctly and generates a warning in developer builds of BigPEmu.
	//normal release builds don't handle this behavior (in the interest of avoiding usually-unnecessary overhead), so the actual software fix is to patch the load into a loadw.
	const uint32_t op = bigpemu_jag_read16(pGame->mRamPatchAddr);
	bigpemu_jag_write16(pGame->mRamPatchAddr, (op & 0x3FF) | (40 << 10));
}

static uint32_t on_sw_loaded(const int eventHandle, void *pEventData)
{
	const uint64_t hash = bigpemu_get_loaded_fnv1a64();
	for (uint32_t gameIndex = 0; gameIndex < skSupportedGameCount; ++gameIndex)
	{
		const TSupportedGameEntry *pGame = &skSupportedGames[gameIndex];
		if (hash == pGame->mSwHash)
		{
			bigpemu_jag_m68k_bp_add(pGame->mManCpuBpAddr, man_cpu_bp);
			bigpemu_jag_blitter_set_excycles(0); //some of these sequences also flicker if the blitter isn't running fast enough, so go ahead and hack that up too while we're at it
			sGameIndex = gameIndex;
			break;
		}
	}
	return 0;
}

void bigp_init()
{
	void *pMod = bigpemu_get_module_handle();

	bigpemu_set_module_usage_flags(pMod, BIGPEMU_MODUSAGE_DETERMINISMWARNING);

	sOnLoadEvent = bigpemu_register_event_sw_loaded(pMod, on_sw_loaded);
}

void bigp_shutdown()
{
	void *pMod = bigpemu_get_module_handle();
	bigpemu_unregister_event(pMod, sOnLoadEvent);
	sOnLoadEvent = -1;
}
