//__BIGPEMU_SCRIPT_MODULE__
//__BIGPEMU_META_DESC__		"Enable tracker/VR functionality for Missile Command 3D."
//__BIGPEMU_META_AUTHOR__	"Rich Whitehouse"

#include "bigpcrt.h"

#define MC3D_HASH					0x8A324BEEC06140ABULL
#define MC3D_CONTROL_WORD_ADDR		0x000486B2

#define MC3D_GPU_BASE_UPLOADED_PC	0x00006418
#define MC3D_GPU_UPLOADED_PC		0x00F03056
#define MC3D_VINT_PC				0x0000985A

//kinda guessing at a few things. this is all reverse engineered, as i'm unaware of any MC3D source code floating around
#define MC3D_CAM_DATA_ADDR			0x0005B0DC
#define MC3D_SPAWNLASER_ADDR		0x0005A01C
#define MC3D_CURPROG_ADDR			0x00F0306C
#define MC3D_DISPLIST_ADDR			0x00F03070 //not exactly a display list, rather a gpu program list
#define MC3D_DISPLIST_VMODE_ADDR	0x0005A8A8
#define MC3D_DISPLIST_3DMODE_ADDR	0x0005A9A8
#define MC3D_DISPLIST_VMODE1_ADDR	0x0005AF38 //and so on, lot of variants for bosses and such
#define MC3D_DISPLIST_VMODECRD_ADDR	0x0005ACA8

#define MC3D_GPUPROG_FIRST_ADDR		0x000459C8
#define MC3D_GPUPROG_RGROUND_ADDR	0x0000E768
#define MC3D_GPUPROG_RENDER_ADDR	0x00010600
#define MC3D_GPUPROG_OLP_ADDR		0x0000F2F0
#define MC3D_GPUPROG_READJOY_ADDR	0x000486F8
#define MC3D_GPUPROG_UPDATECAM_ADDR	0x00028578
#define MC3D_GPUPROG_TFORMLIST_ADDR	0x0000FD28

#define MC3D_STEREO_LIST_TYPE_UNSUPPORTED	0
#define MC3D_STEREO_LIST_TYPE_A				1
#define MC3D_STEREO_LIST_TYPE_B				2

static int32_t sOnLoadEvent = -1;
static int32_t sOnEmuFrame = -1;
static int32_t sOnVideoEvent = -1;
static int32_t sMcFlags = 0;
static const int32_t skMcFlag_Loaded = (1 << 0);
static const uint16_t skVRControlBitsSet = ((1 << 2) | (1 << 3));
static const uint16_t skVRControlBitsUnset = ((1 << 10) | (1 << 1));
static const uint16_t skVRControlReadyBits = ((1 << 4) | (1 << 5));

static int32_t sStereoEnabled = 1;
static int32_t sStereoSeparation = 32;
static int32_t sStereoFocalDistance = 0;

static int32_t sStereoEnabledHandle = -1;
static int32_t sStereoSeparationHandle = -1;
static int32_t sStereoFocalDistanceHandle = -1;

static uint32_t sOddFrame = 0;
static uint32_t sPrevCamPosValid = 0;
static uint32_t sPrevCamRotValid = 0;
static uint32_t sPreserveLaser = 0;
static int32_t sPrevCamPos[3] = { 0, 0, 0 };
static int32_t sPrevCamRot[9] = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };

static uint32_t get_stereo_supported_list_type()
{
	//we could potentially expand this, but may have to handle skipping for the second eye a bit different for different program list setups.
	//this layout generally covers all of the in-game virtual/3d-mode scenes.
	const uint32_t dispAddr = bigpemu_jag_read32(MC3D_DISPLIST_ADDR);
	if (dispAddr &&
		bigpemu_jag_read32(dispAddr) == MC3D_GPUPROG_FIRST_ADDR)
	{
		if (bigpemu_jag_read32(dispAddr + 72) == MC3D_GPUPROG_RGROUND_ADDR && bigpemu_jag_read32(dispAddr + 88) == MC3D_GPUPROG_RENDER_ADDR)
		{
			return MC3D_STEREO_LIST_TYPE_A;
		}
		else if (bigpemu_jag_read32(dispAddr + 80) == MC3D_GPUPROG_RGROUND_ADDR && bigpemu_jag_read32(dispAddr + 96) == MC3D_GPUPROG_RENDER_ADDR && dispAddr != MC3D_DISPLIST_VMODECRD_ADDR)
		{
			return MC3D_STEREO_LIST_TYPE_B;
		}
	}
	return MC3D_STEREO_LIST_TYPE_UNSUPPORTED;
}

static uint32_t eye_skip_size_for_list_type(const uint32_t listType)
{
	switch (listType)
	{
	case MC3D_STEREO_LIST_TYPE_A:
		return 64;
	case MC3D_STEREO_LIST_TYPE_B:
		return 72;
	default:
		break;
	}
	return 0;
}

static void update_stereo_coords()
{
	const float kRotQ = 16384.0f;
	const float kFNorm = 1.0f / kRotQ;
		
	sPrevCamPos[0] = bigpemu_jag_read32(MC3D_CAM_DATA_ADDR);
	sPrevCamPos[1] = bigpemu_jag_read32(MC3D_CAM_DATA_ADDR + 4);
	sPrevCamPos[2] = bigpemu_jag_read32(MC3D_CAM_DATA_ADDR + 8);
	sPrevCamPosValid = 1;
	
	int32_t rightAxis[3];
	rightAxis[0] = (int16_t)bigpemu_jag_read16(MC3D_CAM_DATA_ADDR + 12);
	rightAxis[1] = (int16_t)bigpemu_jag_read16(MC3D_CAM_DATA_ADDR + 14);
	rightAxis[2] = (int16_t)bigpemu_jag_read16(MC3D_CAM_DATA_ADDR + 16);

	const float offsetSign = (sOddFrame) ? -1.0f : 1.0f;
	const float rightScale = (float)sStereoSeparation * offsetSign;
	
	float fRight[3];
	fRight[0] = kFNorm * rightScale * rightAxis[0];
	fRight[1] = kFNorm * rightScale * rightAxis[1];
	fRight[2] = kFNorm * rightScale * rightAxis[2];

	const float centerPos[3] = { (float)sPrevCamPos[0], (float)sPrevCamPos[1], (float)sPrevCamPos[2] };	
	float fNewPos[3];

	math_vec3_add2(fNewPos, centerPos, fRight);
	
	//if a focal distance as specified, adjust the rotation matrix to point from the adjusted eye to the focal point.
	//making this too close of course means you'll be cross-eyed.
	if (sStereoFocalDistance)
	{		
		float fFwd[3];
		float fUp[3];

		sPrevCamRot[0] = rightAxis[0];
		sPrevCamRot[1] = rightAxis[1];
		sPrevCamRot[2] = rightAxis[2];
		sPrevCamRot[3] = (int16_t)bigpemu_jag_read16(MC3D_CAM_DATA_ADDR + 18);
		sPrevCamRot[4] = (int16_t)bigpemu_jag_read16(MC3D_CAM_DATA_ADDR + 20);
		sPrevCamRot[5] = (int16_t)bigpemu_jag_read16(MC3D_CAM_DATA_ADDR + 22);
		sPrevCamRot[6] = (int16_t)bigpemu_jag_read16(MC3D_CAM_DATA_ADDR + 24);
		sPrevCamRot[7] = (int16_t)bigpemu_jag_read16(MC3D_CAM_DATA_ADDR + 26);
		sPrevCamRot[8] = (int16_t)bigpemu_jag_read16(MC3D_CAM_DATA_ADDR + 28);
		sPrevCamRotValid = 1;
				
		fFwd[0] = kFNorm * sPrevCamRot[3];
		fFwd[1] = kFNorm * sPrevCamRot[4];
		fFwd[2] = kFNorm * sPrevCamRot[5];
		fUp[0] = kFNorm * sPrevCamRot[6];
		fUp[1] = kFNorm * sPrevCamRot[7];
		fUp[2] = kFNorm * sPrevCamRot[8];
			
		float focalPos[3];
		float focalVec[3];
		
		math_vec3_copy(focalVec, fFwd);
		math_vec3_scalef(focalVec, (float)sStereoFocalDistance);
		math_vec3_add2(focalPos, centerPos, focalVec);
		
		float newFwd[3];
		math_vec3_sub2(newFwd, focalPos, fNewPos);
		math_vec3_normalize(newFwd);
		float newRight[3];
		math_vec3_cross(newRight, newFwd, fUp);
		
		math_vec3_scalef(newFwd, kRotQ);
		math_vec3_clampf(newFwd, -kRotQ, kRotQ);
		math_vec3_scalef(newRight, kRotQ);
		math_vec3_clampf(newRight, -kRotQ, kRotQ);

		bigpemu_jag_write16(MC3D_CAM_DATA_ADDR + 12, (int16_t)newRight[0]);
		bigpemu_jag_write16(MC3D_CAM_DATA_ADDR + 14, (int16_t)newRight[1]);
		bigpemu_jag_write16(MC3D_CAM_DATA_ADDR + 16, (int16_t)newRight[2]);
		bigpemu_jag_write16(MC3D_CAM_DATA_ADDR + 18, (int16_t)newFwd[0]);
		bigpemu_jag_write16(MC3D_CAM_DATA_ADDR + 20, (int16_t)newFwd[1]);
		bigpemu_jag_write16(MC3D_CAM_DATA_ADDR + 22, (int16_t)newFwd[2]);
	}
	
	bigpemu_jag_write32(MC3D_CAM_DATA_ADDR, (int32_t)fNewPos[0]);
	bigpemu_jag_write32(MC3D_CAM_DATA_ADDR + 4, (int32_t)fNewPos[1]);
	bigpemu_jag_write32(MC3D_CAM_DATA_ADDR + 8, (int32_t)fNewPos[2]);
	
	sOddFrame ^= 1;
}

static void mc3d_gpu_uploaded_gpubp(const uint32_t addr)
{
	bigpemu_jag_gpu_curbank_set_reg(20, 0x00F03080);
	bigpemu_jag_write32(bigpemu_jag_gpu_curbank_get_reg(0), bigpemu_jag_gpu_curbank_get_reg(1));
	
	const uint32_t listType = (sStereoEnabled) ? get_stereo_supported_list_type() : MC3D_STEREO_LIST_TYPE_UNSUPPORTED;
	if (listType != MC3D_STEREO_LIST_TYPE_UNSUPPORTED)
	{
		bigpemu_jag_set_stereo_enabled(1);
		
		const uint32_t srcProg = bigpemu_jag_gpu_curbank_get_reg(2);
		switch (srcProg)
		{
		case MC3D_GPUPROG_FIRST_ADDR:
			if (sOddFrame)
			{
				update_stereo_coords();
				//skip most of the frame, down to the clear/transform/render. this locks logic in place at 30Hz. (without hardware perf cap)
				//we're effectively halting all logic increments on the odd frames, so that we can re-render them as-is with our stereo displacement.
				bigpemu_jag_write32(MC3D_CURPROG_ADDR, bigpemu_jag_read32(MC3D_CURPROG_ADDR) + eye_skip_size_for_list_type(listType));
			}
			break;
		case MC3D_GPUPROG_READJOY_ADDR:
			update_stereo_coords();
			break;
		case MC3D_GPUPROG_OLP_ADDR:
			//the laser is a special case, handled in the same gpu program that updates the main op bitmap data address.
			//if we don't preserve the laser flag across frames, the laser will only render in one eye.
			if (sOddFrame)
			{
				sPreserveLaser = bigpemu_jag_read32(MC3D_SPAWNLASER_ADDR);
			}
			else if (sPreserveLaser)
			{
				bigpemu_jag_write32(MC3D_SPAWNLASER_ADDR, sPreserveLaser);
				sPreserveLaser = 0;
			}
			
			//if anyone is ever kind enough to donate a real Jaguar VR or take one apart and thoroughly document the hardware, we may find there's
			//an existing hardware function for stereo synchronization despite the fact that stereoscopic rendering was never implemented in any
			//software title. or maybe we won't. until then, we just directly tell the emulator which eye is being scanned out.
			bigpemu_jag_set_stereo_scan_eye(sOddFrame); //intentionally flipped from offset logic				
		
			//intentionally fall through to MC3D_GPUPROG_UPDATECAM_ADDR
		case MC3D_GPUPROG_UPDATECAM_ADDR:
			if (sPrevCamPosValid)
			{
				bigpemu_jag_write32(MC3D_CAM_DATA_ADDR, sPrevCamPos[0]);
				bigpemu_jag_write32(MC3D_CAM_DATA_ADDR + 4, sPrevCamPos[1]);
				bigpemu_jag_write32(MC3D_CAM_DATA_ADDR + 8, sPrevCamPos[2]);
				sPrevCamPosValid = 0;
			}
			if (sPrevCamRotValid)
			{
				bigpemu_jag_write16(MC3D_CAM_DATA_ADDR + 12, (int16_t)sPrevCamRot[0]);
				bigpemu_jag_write16(MC3D_CAM_DATA_ADDR + 14, (int16_t)sPrevCamRot[1]);
				bigpemu_jag_write16(MC3D_CAM_DATA_ADDR + 16, (int16_t)sPrevCamRot[2]);
				bigpemu_jag_write16(MC3D_CAM_DATA_ADDR + 18, (int16_t)sPrevCamRot[3]);
				bigpemu_jag_write16(MC3D_CAM_DATA_ADDR + 20, (int16_t)sPrevCamRot[4]);
				bigpemu_jag_write16(MC3D_CAM_DATA_ADDR + 22, (int16_t)sPrevCamRot[5]);
				bigpemu_jag_write16(MC3D_CAM_DATA_ADDR + 24, (int16_t)sPrevCamRot[6]);
				bigpemu_jag_write16(MC3D_CAM_DATA_ADDR + 26, (int16_t)sPrevCamRot[7]);
				bigpemu_jag_write16(MC3D_CAM_DATA_ADDR + 28, (int16_t)sPrevCamRot[8]);
				sPrevCamRotValid = 0;
			}
			break;
		default:
			break;
		}
	}
	else
	{
		bigpemu_jag_set_stereo_enabled(0);
		sOddFrame = 0;
		sPrevCamPosValid = 0;
		sPrevCamRotValid = 0;
		sPreserveLaser = 0;
	}
}

static void mc3d_gpu_base_bp(const uint32_t addr)
{
	bigpemu_jag_inject_risc_bp(MC3D_GPU_UPLOADED_PC, mc3d_gpu_uploaded_gpubp);
}

static uint32_t on_sw_loaded(const int eventHandle, void *pEventData)
{
	const uint64_t hash = bigpemu_get_loaded_fnv1a64();
	
	sMcFlags = 0;
	
	if (hash == MC3D_HASH)
	{
		sMcFlags |= skMcFlag_Loaded;
		
		bigpemu_jag_m68k_bp_add(MC3D_GPU_BASE_UPLOADED_PC, mc3d_gpu_base_bp);
	}
	return 0;
}

static uint32_t on_emu_frame(const int eventHandle, void *pEventData)
{
	if (sMcFlags & skMcFlag_Loaded)
	{
		if (sStereoEnabled)
		{
			//speed up emulation since we're basically cutting framerate in half (albeit not performing a lot of extra work on the odd frames, since we don't advance game state)
			bigpemu_jag_gpu_set_pipeline_enabled(0);
			bigpemu_jag_blitter_set_excycles(0);
			bigpemu_jag_set_risc_clock_scale(2.0);
		}
	
		//don't do anything unless the second input device type is a head tracker
		if (bigpemu_jag_get_device_type(1) == kBPE_DT_HeadTracker)
		{
			const uint16_t cw = bigpemu_jag_read16(MC3D_CONTROL_WORD_ADDR);
			if (cw != 0xFFFF && (cw & skVRControlReadyBits))
			{
				if (!(cw & skVRControlBitsSet) || (cw & skVRControlBitsUnset))
				{
					bigpemu_jag_write16(MC3D_CONTROL_WORD_ADDR, ((cw & ~skVRControlBitsUnset) | skVRControlBitsSet));
				}
				//the game's x center changed between the beta and the unused tracker code in final.
				//previously, x and y were the same. this may be happenstance based on development setup and some hardcoding that might've had to change if VR support were finished.
				bigpemu_jag_set_tracker_constraints_x(-12288, 3);
			}
		}
	}
	return 0;
}

static uint32_t on_video_frame(const int eventHandle, void *pEventData)
{
	bigpemu_get_setting_value(&sStereoEnabled, sStereoEnabledHandle);
	bigpemu_get_setting_value(&sStereoSeparation, sStereoSeparationHandle);
	bigpemu_get_setting_value(&sStereoFocalDistance, sStereoFocalDistanceHandle);
	return 0;
}

void bigp_init()
{
	void *pMod = bigpemu_get_module_handle();
	
	const int catHandle = bigpemu_register_setting_category(pMod, "MC3D");
	sStereoEnabledHandle = bigpemu_register_setting_bool(catHandle, "Stereosopic Mode", 0);
	sStereoSeparationHandle = bigpemu_register_setting_int_full(catHandle, "Eye Separation", 32, 1, 16384, 1);
	sStereoFocalDistanceHandle = bigpemu_register_setting_int_full(catHandle, "Focal Distance", 0, 0, 100000, 10);
	
	sOnLoadEvent = bigpemu_register_event_sw_loaded(pMod, on_sw_loaded);
	sOnEmuFrame = bigpemu_register_event_emu_thread_frame(pMod, on_emu_frame);
	sOnVideoEvent = bigpemu_register_event_video_thread_frame(pMod, on_video_frame);
}

void bigp_shutdown()
{
	void *pMod = bigpemu_get_module_handle();
	bigpemu_unregister_event(pMod, sOnLoadEvent);
	sOnLoadEvent = -1;
	bigpemu_unregister_event(pMod, sOnEmuFrame);
	sOnEmuFrame = -1;
	bigpemu_unregister_event(pMod, sOnVideoEvent);
	sOnVideoEvent = -1;
	sStereoEnabledHandle = -1;
	sStereoSeparationHandle = -1;
	sStereoFocalDistanceHandle = -1;
}
