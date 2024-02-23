//__BIGPEMU_SCRIPT_MODULE__
//__BIGPEMU_META_DESC__		"Rotary controller patches for Tempest 2000."
//__BIGPEMU_META_AUTHOR__	"Rich Whitehouse"

#include "bigpcrt.h"
#include "jagregs.h"

static int32_t sOnLoadEvent = -1;

#define T2K_SUPPORTED_HASH						0x0030407D0269CE03ULL

#define T2K_ROT_DEC_SPEED						16384 //deceleration rate for bonus games under rotary control

#define T2K_PCADDR_SET_SYSFLAGS					0x0080206C
#define T2K_RAM_SYSFLAGS						0x000080B0
#define T2K_RAM_INPUT0							0x00008488
#define T2K_RAM_ROT_TUNNEL						0x00007D1C
#define T2K_RAM_ROT_BACON						0x0000EA8A
#define T2K_PCADDR_ROTARY_TEXT_INPUT_FIXUP_0	0x00806918
#define T2K_PCADDR_ROTARY_TEXT_INPUT_FIXUP_1	0x008069DE
#define T2K_PCADDR_ROTARY_DECEL_0				0x00805978
#define T2K_PCADDR_ROTARY_DECEL_1				0x00804C92

static uint32_t t2k_in_rotary_mode()
{
	const uint8_t sysFlags = bigpemu_jag_read8(T2K_RAM_SYSFLAGS);
	return (sysFlags & (1 << 3));
}

//automatically enable the rotary controller flag if the input type is set to rotary in the emulator
static void t2k_set_sysflags_bp(const uint32_t addr)
{
	if (bigpemu_jag_get_device_type(0) == kBPE_DT_Rotary)
	{
		bigpemu_jag_write8(T2K_RAM_SYSFLAGS, bigpemu_jag_read8(T2K_RAM_SYSFLAGS) | (1 << 3));
	}
}

//make it possible to control the highscore initials entry screen with rotary control
static void t2k_rotary_text_input_fixup_bp(const uint32_t addr)
{
	if (!t2k_in_rotary_mode())
	{
		return;
	}
	
	const uint32_t buttonsOnly = (addr == T2K_PCADDR_ROTARY_TEXT_INPUT_FIXUP_0);

	uint32_t p0Input = bigpemu_jag_read32(T2K_RAM_INPUT0);
	const uint32_t d0 = bigpemu_jag_m68k_get_dreg(0);
	
	if (buttonsOnly)
	{
		p0Input |= d0;
	}
	else
	{
		p0Input |= ((d0 >> 8) & 0xFF);
		float anal[8];
		if (bigpemu_jag_get_analogs(anal, 0) >= 2)
		{
			//the l/r/d/u/etc. bits here don't correspond to the hardware layout (and we're already 8 bits off)
			const uint32_t udBits = (anal[1] < 0.0f) ? (1 << 4) : ((anal[1] > 0.0f) ? (1 << 5) : 0);
			const uint32_t lrBits = (anal[0] < 0.0f) ? (1 << 6) : ((anal[0] > 0.0f) ? (1 << 7) : 0);
			p0Input |= (udBits | lrBits);
		}
	}
	
	bigpemu_jag_m68k_set_dreg(0, p0Input);
}

//make bonus games playable with rotary control
static void t2k_rotary_decel_bp(const uint32_t addr)
{
	if (!t2k_in_rotary_mode())
	{
		return;
	}
	
	const uint32_t rotAddr = (addr == T2K_PCADDR_ROTARY_DECEL_1) ? T2K_RAM_ROT_TUNNEL : T2K_RAM_ROT_BACON;
	int32_t rotCum = (int32_t)bigpemu_jag_read32(rotAddr);
	if (rotCum)
	{
		if (rotCum > 0)
		{
			rotCum = BIGPEMU_MAX(rotCum - T2K_ROT_DEC_SPEED, 0);
		}
		else
		{
			rotCum = BIGPEMU_MIN(rotCum + T2K_ROT_DEC_SPEED, 0);
		}
		bigpemu_jag_write32(rotAddr, rotCum);
	}
}

static uint32_t on_sw_loaded(const int eventHandle, void *pEventData)
{
	const uint64_t hash = bigpemu_get_loaded_fnv1a64();
	if (hash == T2K_SUPPORTED_HASH)
	{
		bigpemu_jag_m68k_bp_add(T2K_PCADDR_SET_SYSFLAGS, t2k_set_sysflags_bp);
		bigpemu_jag_m68k_bp_add(T2K_PCADDR_ROTARY_TEXT_INPUT_FIXUP_0, t2k_rotary_text_input_fixup_bp);
		bigpemu_jag_m68k_bp_add(T2K_PCADDR_ROTARY_TEXT_INPUT_FIXUP_1, t2k_rotary_text_input_fixup_bp);
		bigpemu_jag_m68k_bp_add(T2K_PCADDR_ROTARY_DECEL_0, t2k_rotary_decel_bp);
		bigpemu_jag_m68k_bp_add(T2K_PCADDR_ROTARY_DECEL_1, t2k_rotary_decel_bp);
	}
	return 0;
}

void bigp_init()
{
	void *pMod = bigpemu_get_module_handle();
	
	sOnLoadEvent = bigpemu_register_event_sw_loaded(pMod, on_sw_loaded);
}

void bigp_shutdown()
{
	void *pMod = bigpemu_get_module_handle();
	bigpemu_unregister_event(pMod, sOnLoadEvent);
	sOnLoadEvent = -1;
}
