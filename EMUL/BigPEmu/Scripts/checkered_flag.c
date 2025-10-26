//__BIGPEMU_SCRIPT_MODULE__
//__BIGPEMU_META_DESC__		"Checkered Flag enhancements."
//__BIGPEMU_META_AUTHOR__	"Rich Whitehouse"

#include "bigpcrt.h"
#include "jagregs.h"

static int32_t sOnLoadEvent = -1;
static int32_t sOnVideoEvent = -1;
static int32_t sPreUIEvent = -1;
static int32_t sOnStateSaveEvent = -1;
static int32_t sOnStateLoadEvent = -1;

static int32_t sNativeRenderSettingHandle = -1;
static int32_t sTexFilterSettingHandle = -1;
static int32_t sCRYBlendingSettingHandle = -1;
static int32_t sUncapFramerateSettingHandle = -1;
static int32_t sAnalogSteeringSettingHandle = -1;
static int32_t sAnalogExponentSettingHandle = -1;
static int32_t sAnalogAccelSpeedSettingHandle = -1;
static int32_t sAnalogDecelSpeedSettingHandle = -1;
static int32_t sAnalogConstantLimitSettingHandle = -1;
static int32_t sAnalogLimitScaleSettingHandle = -1;
static int32_t sAnalogCamSwingSettingHandle = -1;
static int32_t sDeltaSpeedCorSettingHandle = -1;
static int32_t sCamTiltSettingHandle = -1;
static int32_t sSkidNoiseSettingHandle = -1;
static int32_t sDrawRainSettingHandle = -1;

#define CF_NATIVE_RES_DEFAULT				1
#define CF_TEX_FILTER_DEFAULT				1
#define CF_CRY_BLENDING_DEFAULT				1
#define CF_UNCAPFRAMERATE_DEFAULT			0
#define CF_ANALOG_STEERING_DEFAULT			1
#define CF_ANALOG_EXPONENT_DEFAULT			2.0f
#define CF_ANALOG_ACCEL_SPEED_DEFAULT		8.0f
#define CF_ANALOG_DECEL_SPEED_DEFAULT		16.0f
#define CF_ANALOG_CONSTANT_LIMIT_DEFAULT	0.0f
#define CF_ANALOG_LIMIT_SCALE_DEFAULT		1.0f
#define CF_ANALOG_CAMSWING_DEFAULT			0
#define CF_DELTASPEEDCOR_DEFAULT			1
#define CF_CAMTILT_DEFAULT					1
#define CF_SKIDNOISE_DEFAULT				1
#define CF_DRAWRAIN_DEFAULT					0

static int32_t sLastNativeRender = CF_NATIVE_RES_DEFAULT;
static int32_t sLastTexFilter = CF_TEX_FILTER_DEFAULT;
static int32_t sLastCRYBlending = CF_CRY_BLENDING_DEFAULT;
static int32_t sLastUncapFramerate = CF_UNCAPFRAMERATE_DEFAULT;
static int32_t sLastAnalogSteering = CF_ANALOG_STEERING_DEFAULT;
static float sLastAnalogExponent = CF_ANALOG_EXPONENT_DEFAULT;
static float sLastAnalogAccelSpeed = CF_ANALOG_ACCEL_SPEED_DEFAULT;
static float sLastAnalogDecelSpeed = CF_ANALOG_DECEL_SPEED_DEFAULT;
static float sLastAnalogConstantLimit = CF_ANALOG_CONSTANT_LIMIT_DEFAULT;
static float sLastAnalogLimitScale = CF_ANALOG_LIMIT_SCALE_DEFAULT;
static int32_t sLastAnalogCamSwing = CF_ANALOG_CAMSWING_DEFAULT;
static int32_t sLastDeltaSpeedCor = CF_DELTASPEEDCOR_DEFAULT;
static int32_t sLastCamTilt = CF_CAMTILT_DEFAULT;
static int32_t sLastSkidNoise = CF_SKIDNOISE_DEFAULT;
static int32_t sLastDrawRain = CF_DRAWRAIN_DEFAULT;

#define CF_SAVE_VERSION						1

//this data is preserved as part of emulator saved states
typedef struct
{
	int32_t mSaveVersion;
	int32_t mGameLoaded;
	int32_t mCurrentFbAddr;
	int32_t mCurrentSecGPUAddr;
	int32_t mInGame;
	uint32_t mWaitingForAnalogInput;
	float mAnalogSteering;
	float mLastAnalogYaw;
	float mCurDelta;
	float mDeltaScale;
	uint32_t mRainColorIndex;
	uint32_t mResv1;
	uint32_t mResv2;
	uint32_t mResv3;
	double mPrevTime;
} SCFGameState;
static SCFGameState sGS;

#define CF_SUPPORTED_HASH					0xF9DDA93597C567F7ULL
#define CF_GPU_SEC_UPLOAD_PC				0x00F03056
#define CF_GPU_SEC_UPLOAD_KICK_PC			0x00F0306E
#define CF_GPU_SEC_FX_ADDR					0x00020110

#define CF_NATIVE_RENDER_TIME_WINDOW		500.0 //in microseconds

#define CF_STEERING_RANGE					50.0f
#define CF_MIN_TIME_DELTA					8.0
#define CF_MAX_TIME_DELTA					100.0f

#define CF_FB_ADDR							0x000E8000
#define CF_FB_OFFSET_ADDR					0x00004044
#define CF_FB_WIDTH							320
#define CF_FB_HEIGHT						242
#define CF_POLYREG_HEIGHT					230
#define CF_SKY_WIDTH						320
#define CF_SKY_HEIGHT						125
#define CF_SKY_PITCH						1024
#define CF_SKY_TEX_ADDR						0x00140000
#define CF_SKY_XOFFSET_ADDR					0x00059002
#define CF_SKY_YOFFSET_ADDR					0x00004390
#define CF_SKY_COLOR_ADDR					0x000040CC
#define CF_SKY_TEXTURE_COUNT				7
#define CF_STEER_ADDR						0x0000455E
#define CF_INPUT_ADDR						0x00004068
#define CF_SPEED_ADDR						0x0000405C
#define CF_CAM_TILT_PARAM0_ADDR				0x0000437C
#define CF_CAM_TILT_PARAM1_ADDR				0x0000437E
#define CF_CAM_TILT_PARAM2_ADDR				0x00004380
#define CF_CAM_TILT_PARAM3_ADDR				0x00004382
#define CF_CAM_TILT_PARAM4_ADDR				0x00004384
#define CF_CAM_TILT_PARAM5_ADDR				0x00004386
#define CF_FRAMELIMIT_ADDR					0x000043B0
#define CF_SFX_SKIDNOISE_ADDR				0x00813E44
#define CF_MAPTYPE_ADDR						0x00004450

#define CF_UPLOAD_GPU_DRAW_CODE_PC			0x00802852
#define CF_DRAW_SKY_LOOPBACK_PC				0x000D77B2
#define CF_DRAW_SKY_CODE_PC					0x000D77B8
#define CF_DRAW_SKY_RET_PC					0x000D78AA
#define CF_BUFFER_FLIP_PC					0x000D675E
#define CF_GAME_BEGIN_PC					0x000D615C
#define CF_GAME_END_PC						0x000D64E0
#define CF_RAIN_CHECK_PC					0x000D69FC
#define CF_RAIN_NORAIN_PC					0x000D6A1A
#define CF_UPDATECLUT_PC					0x000D7F12
#define CF_UPDATECLUT_RET_PC				0x000D7F2A
#define CF_CHECK_STEER_INPUT_PC				0x000DB304
#define CF_STEER_RIGHT_PC					0x000DB350
#define CF_STEER_LEFT_PC					0x000DB3A0
#define CF_STEER_RET_PC						0x000DB336
#define CF_SET_SPEED_PC						0x000DACFA
#define CF_SET_SPEED_REENTRY_PC				0x000DAD04
#define CF_ADJUST_SPEED_FOR_TILT_PC			0x000DA7CC
#define CF_GAME_FRAME_PC					0x000D6708
#define CF_CAMERA_SWING_PC					0x000DAB6A
#define CF_TILT_ADJUST_PC					0x000DA7B6
#define CF_STARTLOOP_PC						0x000D62B2
#define CF_GAMELOOP_PC						0x000D63B4
#define CF_RENDERLOOP_PC					0x000D66F2
#define CF_PLAYSOUND_PC						0x008131B6
#define CF_PLAYSOUND_RET_PC					0x0081329A
#define CF_DRAWMAP_PC						0x000DD34A
#define CF_GPU_DECOMP_BYTE_PC				0x000D8CF6
#define CF_GPU_DECOMP_WORD_PC				0x000DCF44

#define CF_GPU_DRAW_POLY_PC					0x00F0342A
#define CF_GPU_SEC_DRAW_BITMAP_PC			0x00F03B78

#define CF_VERT_UVF(pVert, x, y, c, u, v) \
	((float *)pVert)[0] = x; \
	((float *)pVert)[1] = y; \
	((float *)pVert)[2] = 0.0f; \
	((uint32_t *)pVert)[3] = c; \
	((float *)pVert)[4] = u; \
	((float *)pVert)[5] = v;

#define CF_VERT_NTX(pVert, x, y, c) \
	((float *)pVert)[0] = x; \
	((float *)pVert)[1] = y; \
	((float *)pVert)[2] = 0.0f; \
	*(uint16_t *)((uint32_t *)pVert + 3) = c;

#define CF_UVF_POINT_SIZE 		(6 * 4)
#define CF_NTX_POINT_SIZE 		(4 * 4)
#define CF_MAX_POINT_SIZE 		CF_UVF_POINT_SIZE
#define CF_MAX_POINT_COUNT		256

static uint8_t sPointBuffer[CF_MAX_POINT_COUNT * CF_MAX_POINT_SIZE];
static uint8_t sTempFramebuffer[CF_FB_WIDTH * CF_FB_HEIGHT * 2];

static float sNativeWidth = CF_FB_WIDTH;
static float sNativeHeight = CF_FB_HEIGHT;

typedef struct 
{
	uint32_t mWidth;
	uint32_t mHeight;
	TSharedResPtr mResPtr;
} SSkyTextureOverride;

static const uint32_t skSkyTextureAddresses[CF_SKY_TEXTURE_COUNT] =
{
	0x008CD722, //generic rain/fog
	0x0093252C, //island hop, deep wood
	0x00940794, //the hole, concrete canyon
	0x00955260, //arctic run
	0x00963AE4, //green valley, river mouth
	0x00971720, //the gorge, desert pass
	0x0097D6A8 //sunset strip
};

//we're only allowed to deal with native resources on the main thread, so we so this op value to issue loads/frees from other threads
typedef enum
{
	kSkyOp_None = 0,
	kSkyOp_Load,
	kSkyOp_Free
} ESkyOp;

static SSkyTextureOverride sSkyTextureOverrides[CF_SKY_TEXTURE_COUNT];
static int32_t sSkyTextureOverrideIndex = -1;
static ESkyOp sSkyOp = kSkyOp_None;

static double get_abs_time_ms()
{
	return ((double)bigpemu_jag_get_frame_count() * bigpemu_jag_get_frame_period() + bigpemu_jag_get_exec_time()) / 1000.0;
}

static float ndc_x_fl(const float x)
{
	static const float skToNdcX = 1.0f / (float)CF_FB_WIDTH; //0..1
	return x * skToNdcX;
}

static float ndc_x(const int32_t x)
{
	return ndc_x_fl((float)x);
}

static float ndc_y_fl(const float y)
{
	static const float skToNdcY = 1.0f / (float)CF_FB_HEIGHT; //0..1
	return y * skToNdcY;
}

static float ndc_y_poly(const float y)
{
	static const float skToNdcY = 1.0f / (float)CF_POLYREG_HEIGHT; //0..1
	return y * skToNdcY;
}

static float ndc_y(const int32_t y)
{
	return ndc_y_fl((float)y);
}

static void add_quad_tex(const int32_t x, const int32_t y, const int32_t w, const int32_t h, const uint32_t texWidth, const uint32_t texHeight, const uint32_t texPitch, const uint32_t texAddr, const uint32_t extraPolyFlags, void *pNativeTexAddr, TSharedResPtr sharedRes)
{
	int32_t clipPos[2];
	int32_t clipSize[2];
	if (!math_clip_axially_aligned_quad_to_canvas(clipPos, clipSize, x, y, w, h, CF_FB_WIDTH, CF_FB_HEIGHT))
	{
		return;
	}	
	
	float uvRect[8];
	uint32_t *pVerts = (uint32_t *)sPointBuffer;
	SOPVMPolyInfo polyInfo;
	const uint32_t sprColor = 0xFFFFFFFF;
	uint32_t polyFlags = (OPVM_POLYINFO_FLAG_NODEPTHTEST | OPVM_POLYINFO_FLAG_NODEPTHWRITE | extraPolyFlags);
	uint32_t texFlags = (sLastTexFilter) ? OPVM_TEXINFO_FLAG_WANTFILTER : 0;
	if (sLastCRYBlending && sLastDrawRain)
	{
		texFlags |= OPVM_TEXINFO_FLAG_KEEPCRY;
		//if we're preserving cry textures through the gpu pipeline, don't rely on bilinear filtering.
		//although we do split cyan-red-intensity to assist in interpolation, there's still some potential to end up with undesirable magnifications.
		texFlags &= ~OPVM_TEXINFO_FLAG_WANTFILTER;
	}
	const float uvScaleX = 1.0f / (float)w;
	const float uvScaleY = 1.0f / (float)h;
	
	uvRect[0] = (float)(clipPos[0] - x) * uvScaleX;
	uvRect[1] = (float)(clipPos[1] - y) * uvScaleY;
	
	uvRect[2] = (float)((clipPos[0] + clipSize[0]) - x) * uvScaleX;
	uvRect[3] = (float)(clipPos[1] - y) * uvScaleY;
	
	uvRect[4] = (float)((clipPos[0] + clipSize[0]) - x) * uvScaleX;
	uvRect[5] = (float)((clipPos[1] + clipSize[1]) - y) * uvScaleY;
	
	uvRect[6] = (float)(clipPos[0] - x) * uvScaleX;
	uvRect[7] = (float)((clipPos[1] + clipSize[1]) - y) * uvScaleY;
	
	const uint32_t frameTexAddr = (pNativeTexAddr) ? (uint32_t)pNativeTexAddr : texAddr;
	const uint32_t frameTexIsJagAddr = (pNativeTexAddr) ? 0 : 1;
	
	memset(&polyInfo, 0, sizeof(polyInfo));
	polyInfo.mpData = (vmuptr_t)sPointBuffer;
	polyInfo.mVersion = OPVM_POLYINFO_VERSION;
	polyInfo.mPointCount = 4;
	polyInfo.mStride = CF_UVF_POINT_SIZE;
	polyInfo.mPosOffset = 0;
	polyInfo.mUvOffset = 4 * 4;
	polyInfo.mColorOffset = 4 * 3;
	polyInfo.mTexRefIndex = (!sharedRes) ?
	                                       bigpemu_jag_op_create_frame_tex(sGS.mCurrentFbAddr, frameTexAddr, frameTexIsJagAddr, texWidth, texHeight, (texPitch << 1), kOPVMTex_CRY16, texFlags, NULL) :
	                                       bigpemu_jag_op_create_tex_from_res(sGS.mCurrentFbAddr, sharedRes, 0);
	polyInfo.mPosType = kOPVMPos_XyzFloat32;
	polyInfo.mUvType = kOPVMUv_Float32;
	polyInfo.mColorType = kOPVMColor_RGB32;
	polyInfo.mBlendMode = kSpecialPolyBlend_None;
	polyInfo.mFlags = polyFlags;
	
	CF_VERT_UVF(pVerts, ndc_x(clipPos[0]), ndc_y(clipPos[1]), sprColor, uvRect[0], uvRect[1]);
	pVerts += 6;
	CF_VERT_UVF(pVerts, ndc_x(clipPos[0] + clipSize[0]), ndc_y(clipPos[1]), sprColor, uvRect[2], uvRect[3]);
	pVerts += 6;
	CF_VERT_UVF(pVerts, ndc_x(clipPos[0] + clipSize[0]), ndc_y(clipPos[1] + clipSize[1]), sprColor, uvRect[4], uvRect[5]);
	pVerts += 6;
	CF_VERT_UVF(pVerts, ndc_x(clipPos[0]), ndc_y(clipPos[1] + clipSize[1]), sprColor, uvRect[6], uvRect[7]);
	
	bigpemu_jag_op_add_poly(sGS.mCurrentFbAddr, &polyInfo);
}

static void add_quad(const int32_t x, const int32_t y, const int32_t w, const int32_t h, const uint32_t sprColor)
{
	int32_t clipPos[2];
	int32_t clipSize[2];
	if (!math_clip_axially_aligned_quad_to_canvas(clipPos, clipSize, x, y, w, h, CF_FB_WIDTH, CF_FB_HEIGHT))
	{
		return;
	}
	
	uint32_t *pVerts = (uint32_t *)sPointBuffer;
	SOPVMPolyInfo polyInfo;
	uint32_t polyFlags = (OPVM_POLYINFO_FLAG_NODEPTHTEST | OPVM_POLYINFO_FLAG_NODEPTHWRITE);
	if (sLastCRYBlending)
	{
		polyFlags |= OPVM_POLYINFO_FLAG_KEEPCRY;
	}
	
	memset(&polyInfo, 0, sizeof(polyInfo));
	polyInfo.mpData = (vmuptr_t)sPointBuffer;
	polyInfo.mVersion = OPVM_POLYINFO_VERSION;
	polyInfo.mPointCount = 4;
	polyInfo.mStride = CF_NTX_POINT_SIZE;
	polyInfo.mPosOffset = 0;
	polyInfo.mColorOffset = 4 * 3;
	polyInfo.mTexRefIndex = -1;
	polyInfo.mPosType = kOPVMPos_XyzFloat32;
	polyInfo.mColorType = kOPVMColor_CRY16;
	polyInfo.mBlendMode = kSpecialPolyBlend_None;
	polyInfo.mFlags = polyFlags;
	CF_VERT_NTX(pVerts, ndc_x(clipPos[0]), ndc_y(clipPos[1]), sprColor);
	pVerts += 4;
	CF_VERT_NTX(pVerts, ndc_x(clipPos[0] + clipSize[0]), ndc_y(clipPos[1]), sprColor);
	pVerts += 4;
	CF_VERT_NTX(pVerts, ndc_x(clipPos[0] + clipSize[0]), ndc_y(clipPos[1] + clipSize[1]), sprColor);
	pVerts += 4;
	CF_VERT_NTX(pVerts, ndc_x(clipPos[0]), ndc_y(clipPos[1] + clipSize[1]), sprColor);
	
	bigpemu_jag_op_add_poly(sGS.mCurrentFbAddr, &polyInfo);
}

static float cf_coord_to_screen(const int32_t v)
{
	//instead of shifting right 10, we convert to float to preserve sub-"pixel" data (which will be useful for scaling up to native res)
	return (float)v / 1024.0f;
}

static void cf_draw_poly_gpubp(const uint32_t addr)
{
	bigpemu_jag_gpu_set_reg(12, 0x00004221);
	const uint32_t blitRegBase = bigpemu_jag_gpu_get_reg(14);
	const uint32_t cryColor = bigpemu_jag_gpu_get_reg(25);
	bigpemu_jag_write32(blitRegBase + 64, cryColor);
	
	if (sLastNativeRender && sGS.mCurrentFbAddr)
	{
		const uint32_t killAddr = bigpemu_jag_gpu_get_reg(28);
	
		const uint32_t origClr = bigpemu_jag_gpu_altbank_get_reg(0);
		const uint32_t isShadow = (origClr & (1 << 19));
		if (!isShadow || sLastCRYBlending)
		{
			const uint32_t qty = bigpemu_jag_gpu_curbank_get_reg(24);
			uint32_t dataAddr = bigpemu_jag_gpu_altbank_get_reg(25);
				
			uint32_t *pVerts = (uint32_t *)sPointBuffer;
			SOPVMPolyInfo polyInfo;

			memset(&polyInfo, 0, sizeof(polyInfo));
			polyInfo.mpData = (vmuptr_t)sPointBuffer;
			polyInfo.mVersion = OPVM_POLYINFO_VERSION;
			polyInfo.mPointCount = qty;
			polyInfo.mStride = CF_NTX_POINT_SIZE;
			polyInfo.mPosOffset = 0;
			polyInfo.mColorOffset = 4 * 3;
			polyInfo.mTexRefIndex = -1;
			polyInfo.mPosType = kOPVMPos_XyzFloat32;
			polyInfo.mColorType = kOPVMColor_CRY16;
			polyInfo.mBlendMode = kSpecialPolyBlend_None;
			polyInfo.mFlags = (OPVM_POLYINFO_FLAG_NODEPTHTEST | OPVM_POLYINFO_FLAG_NODEPTHWRITE);
			if (sLastCRYBlending)
			{
				polyInfo.mFlags |= OPVM_POLYINFO_FLAG_KEEPCRY;
				if (isShadow)
				{
					polyInfo.mFlags |= OPVM_POLYINFO_FLAG_ADDDSEL;
				}
			}
			
			//"backward" winding
			dataAddr += (qty - 1) * 8;
			const uint32_t beCryColor = BIGPEMU_SWAP32(cryColor);
			for (uint32_t vertIndex = 0; vertIndex < qty; ++vertIndex)
			{
				const int32_t x = bigpemu_jag_read32(dataAddr);
				const int32_t y = bigpemu_jag_read32(dataAddr + 4);
				const float fx = ndc_x_fl(cf_coord_to_screen(x));
				const float fy = ndc_y_poly(cf_coord_to_screen(y));
				dataAddr -= 8;
				
				CF_VERT_NTX(pVerts, fx, fy, beCryColor);
				pVerts += 4;
			}
			
			//create a thin quad to emulate the line
			if (qty == 2)
			{
				memcpy(pVerts, pVerts - 4, sizeof(uint32_t) * 4);
				memcpy(pVerts + 4, pVerts - 8, sizeof(uint32_t) * 4);
				((float *)pVerts)[1] += (1.0f / sNativeHeight);
				pVerts += 4;
				((float *)pVerts)[1] += (1.0f / sNativeHeight);
				polyInfo.mPointCount += 2;
			}
			
			polyInfo.mPointCount = math_clip_2d_polygon_to_ndc(sPointBuffer, CF_NTX_POINT_SIZE, polyInfo.mPointCount);
			if (polyInfo.mPointCount > 2)
			{
				bigpemu_jag_op_add_poly(sGS.mCurrentFbAddr, &polyInfo);
			}
		}
		
		bigpemu_jag_gpu_set_pc(killAddr);
	}
}

static void cf_sec_upload_gpubp(const uint32_t addr)
{
	const uint32_t prgSrcAddr = bigpemu_jag_gpu_get_reg(29);
	const uint32_t blitRegBase = bigpemu_jag_gpu_get_reg(14);
	bigpemu_jag_write32(blitRegBase + 36, prgSrcAddr);
	bigpemu_jag_gpu_set_reg(0, bigpemu_jag_gpu_get_reg(0) | (1 << 16));
	bigpemu_jag_gpu_set_reg(29, 0);	
	bigpemu_jag_write32(blitRegBase + 60, bigpemu_jag_gpu_get_reg(0));
	sGS.mCurrentSecGPUAddr = prgSrcAddr;
}

static void cf_sec_upload_kick_gpubp(const uint32_t addr)
{
	const uint32_t blitRegBase = bigpemu_jag_gpu_get_reg(14);
	bigpemu_jag_gpu_set_reg(0, 0x00F0BA18);
	bigpemu_jag_gpu_set_reg(29, bigpemu_jag_gpu_get_reg(29) + 1);
	bigpemu_jag_write32(blitRegBase, bigpemu_jag_gpu_get_reg(0));
	bigpemu_jag_write32(blitRegBase + 56, bigpemu_jag_gpu_get_reg(29));
	bigpemu_jag_gpu_set_pc(0x00F0307A);
	//possible todo - check sGS.mCurrentSecGPUAddr here to patch in to other types of draws if needed
}

static void cf_upload_draw_code_bp(const uint32_t addr)
{
	bigpemu_jag_inject_risc_bp(CF_GPU_DRAW_POLY_PC, cf_draw_poly_gpubp);
	bigpemu_jag_inject_risc_bp(CF_GPU_SEC_UPLOAD_PC, cf_sec_upload_gpubp);
	bigpemu_jag_inject_risc_bp(CF_GPU_SEC_UPLOAD_KICK_PC, cf_sec_upload_kick_gpubp);
}

static void cf_draw_sky_bp(const uint32_t addr)
{
	if (sLastUncapFramerate)
	{
		bigpemu_jag_gpu_set_pipeline_enabled(0);
		bigpemu_jag_dsp_set_pipeline_enabled(0);
		//she can't take much more of this, captain
		const double clockScale = (sLastUncapFramerate > 30) ? 2.0 : 1.0; //this means 2-4x for the 68k, 1-2x for risc
		bigpemu_jag_set_m68k_clock_scale(clockScale);
		bigpemu_jag_set_risc_clock_scale(clockScale);
	}
	
	if (sLastNativeRender && sGS.mInGame)
	{
		if (bigpemu_jag_get_exec_time() > CF_NATIVE_RENDER_TIME_WINDOW)
		{
			//stall until we're near the start of a scan-out, ensuring we don't end up with a partial geometry list attached to an object that the op is just starting to process
			bigpemu_jag_m68k_set_pc(CF_DRAW_SKY_LOOPBACK_PC);
			bigpemu_jag_m68k_consume_cycles(-1);
			return;
		}
		
		const uint32_t fbAddr = CF_FB_ADDR + bigpemu_jag_read32(CF_FB_OFFSET_ADDR);
		
		bigpemu_jag_op_clear_buffers(fbAddr);
		sGS.mCurrentFbAddr = fbAddr;

		bigpemu_jag_op_set_alpha_fill(1); //make sure alpha is set when converting to rgba32, regardless of pixel mode.
		bigpemu_jag_op_set_special_transparency(fbAddr, 0x80FF, kOPVMColor_CRY16);
		bigpemu_jag_op_enable_play_area_scissor(1);
		bigpemu_jag_blitter_set_excycles(0); //make sure the blitter is set to free
		
		//hack - keep rain invisible since it relies on op blend with a framebuffer that no longer exists in software (possible todo - if rain is enabled, add our own quad effect)
		bigpemu_jag_write16(CLUT + 6, 0);
		
		bigpemu_jag_write32(A1_BASE, fbAddr);
		bigpemu_jag_write32(A1_PIXEL, 0);
		bigpemu_jag_write32(A1_FPIXEL, 0);
		bigpemu_jag_write32(A1_FLAGS, PITCH2 | PIXEL16 | XADDPHR | WID320);
		bigpemu_jag_write32(A1_STEP, 0x10000 | (uint16_t)(-CF_FB_WIDTH));
		bigpemu_jag_write32(A1_FSTEP, 0);
		bigpemu_jag_write32(A1_INC, 1);
		bigpemu_jag_write32(A1_FINC, 0);
		bigpemu_jag_write32(A2_PIXEL, 0);
		bigpemu_jag_write32(B_COUNT, CF_FB_WIDTH | (CF_FB_HEIGHT << 16));
		bigpemu_jag_write32(B_IINC, 0);
		bigpemu_jag_write32(B_PATD, 0x80FF80FF);
		bigpemu_jag_write32(B_PATD + 4, 0x80FF80FF);
		bigpemu_jag_write32(B_SRCD, 0x80FF80FF);
		bigpemu_jag_write32(B_SRCD + 4, 0x80FF80FF);
		bigpemu_jag_write32(B_CMD, PATDSEL | UPDA1);
		
		const int32_t skyXOffset = (bigpemu_jag_read16(CF_SKY_XOFFSET_ADDR) & 0x3FF);
		const int32_t skyYOffset = (BIGPEMU_MAX(0x3C - ((int16_t)bigpemu_jag_read16(CF_SKY_YOFFSET_ADDR)), 0) >> 1);
		const uint32_t skyAddr = CF_SKY_TEX_ADDR + ((skyYOffset * CF_SKY_PITCH + skyXOffset) << 1);
		const uint32_t skyHeight = CF_SKY_HEIGHT - skyYOffset;
		const uint32_t skyQuadYOffset = 7;
		if (sSkyTextureOverrideIndex >= 0)
		{
			//draw from the native resource, intentionally stretching 2 quads outside of the framebuffer so that the canvas clipping automatically adjusts the uv coordinates for us
			const SSkyTextureOverride *pSkyTex = &sSkyTextureOverrides[sSkyTextureOverrideIndex];
			assert(pSkyTex->mResPtr);
			add_quad_tex(-skyXOffset, skyQuadYOffset, CF_SKY_PITCH, CF_FB_HEIGHT, CF_SKY_WIDTH, skyHeight, 0, 0, 0, NULL, pSkyTex->mResPtr);
			add_quad_tex(CF_SKY_PITCH - skyXOffset, skyQuadYOffset, CF_SKY_PITCH, CF_FB_HEIGHT, CF_SKY_WIDTH, skyHeight, 0, 0, 0, NULL, pSkyTex->mResPtr);
		}
		else
		{
			add_quad_tex(0, skyQuadYOffset, CF_FB_WIDTH, skyHeight, CF_SKY_WIDTH, skyHeight, CF_SKY_PITCH, skyAddr, 0, NULL, 0);
			const uint32_t skyColor = BIGPEMU_SWAP32(bigpemu_jag_read32(CF_SKY_COLOR_ADDR));
			add_quad(0, skyQuadYOffset + skyHeight - 1, CF_FB_WIDTH, (CF_FB_HEIGHT - skyHeight) + 2, skyColor);
		}
		bigpemu_jag_m68k_set_pc(CF_DRAW_SKY_RET_PC);
	}
}

static void cf_buffer_flip_bp(const uint32_t addr)
{
	if (sLastNativeRender && sLastDrawRain && sGS.mCurrentFbAddr && sGS.mRainColorIndex)
	{
		//if rain is enabled, render it into our own local buffer and add it as a textured polygon with signed color/intensity blending
		bigpemu_jag_write16(CLUT + 6, sGS.mRainColorIndex);
		const uint32_t tObjAddr = 0x4E20;
		const uint32_t objAddr = 0x5420;
		const uint64_t cw = bigpemu_jag_read64(objAddr);
		const uint64_t tCw = bigpemu_jag_read64(tObjAddr);
		const uint64_t tCw1 = bigpemu_jag_read64(tObjAddr + 8);
		const uint32_t addr = (cw >> 43ULL);
		//back the live address up to determine what the randomized offset was before the object was touched
		const uint32_t heightDelta = ((tCw >> 14ULL) & 1023) - ((cw >> 14ULL) & 1023);
		const uint64_t newAddr = addr - heightDelta * ((tCw1 >> 18ULL) & 1023);
		const uint64_t kCwMask = 0xFFFFF80000000000ULL;
		bigpemu_jag_write64(tObjAddr, (newAddr << 43ULL) | (tCw & ~kCwMask));
		bigpemu_jag_op_render_bitmap_object_to_buffer(sTempFramebuffer, NULL, CF_FB_WIDTH, CF_FB_HEIGHT, tObjAddr, BIGPEMU_OPRFLAG_CLEARBUFFER);
		bigpemu_jag_write64(tObjAddr, tCw);
		bigpemu_jag_write16(CLUT + 6, 0);
		
		add_quad_tex(0, 0, CF_FB_WIDTH, CF_FB_HEIGHT, CF_FB_WIDTH, CF_FB_HEIGHT, CF_FB_WIDTH, 0, OPVM_POLYINFO_FLAG_ADDDSEL_SIGN, sTempFramebuffer, 0);
	}
	//no valid fb target unless we hit the sky draw later in the frame (other scenes aren't handled natively)
	sGS.mCurrentFbAddr = 0;
}


static void cf_game_begin_bp(const uint32_t addr)
{
	sGS.mInGame = 1;
}

static void cf_game_end_bp(const uint32_t addr)
{
	sGS.mInGame = 0;
}

static void cf_updateclut_bp(const uint32_t addr)
{
	if (bigpemu_jag_m68k_get_dreg(1) == 6)
	{
		sGS.mRainColorIndex = (bigpemu_jag_m68k_get_dreg(0) & 0xFFFF);
		if (sLastNativeRender)
		{
			bigpemu_jag_write16(CLUT + 6, 0);
			bigpemu_jag_m68k_set_pc(CF_UPDATECLUT_RET_PC);
		}
	}
}

static void cf_check_steer_bp(const uint32_t addr)
{
	if (!sLastAnalogSteering || sGS.mCurDelta <= 0.0f || (bigpemu_jag_read32(CF_INPUT_ADDR) & ((1 << JOY_LEFT)|(1 << JOY_RIGHT))))
	{
		//don't interfere with pad turning
		sGS.mWaitingForAnalogInput = 1;
		sGS.mAnalogSteering = (int16_t)bigpemu_jag_read16(CF_STEER_ADDR);
		sGS.mLastAnalogYaw = 0.0f;
		return;
	}
	
	const float steeringLimit = fabsf((int16_t)bigpemu_jag_m68k_get_dreg(2));
		
	//check for analog inputs on the first 2 input devices
	uint32_t gotInput = 0;
	float desiredAngle = 0.0f;
	float analYaw = 0.0f;
	for (uint32_t inputIndex = 0; inputIndex < 2; ++inputIndex)
	{
		if (bigpemu_jag_get_device_type(inputIndex) != kBPE_DT_Standard)
		{
			float anal[8];
			if (bigpemu_jag_get_analogs(anal, inputIndex) >= 2)
			{
				analYaw = anal[0];
				if (analYaw)
				{
					sGS.mWaitingForAnalogInput = 0;
				}
				
				if (!sGS.mWaitingForAnalogInput)
				{
					const float fSign = (analYaw < 0.0f) ? -1.0f : 1.0f;
					const float analCurved = powf(fabsf(analYaw), sLastAnalogExponent) * fSign;
					const float steeringRange = (sLastAnalogConstantLimit > 0.01f) ? sLastAnalogConstantLimit : steeringLimit * sLastAnalogLimitScale;
					desiredAngle = -analCurved * steeringRange;
					gotInput = 1;
				}
				break;
			}
		}
	}

	sGS.mLastAnalogYaw = analYaw;
	if (gotInput)
	{
		const float rateFromDelta = sGS.mDeltaScale;	
		const float currentAngle = sGS.mAnalogSteering;
		const float accelSpeed = sLastAnalogAccelSpeed;
		const float decelSpeed = sLastAnalogDecelSpeed;
		const float approachSpeed = (fabsf(currentAngle) < fabsf(desiredAngle)) ? accelSpeed : decelSpeed;
		const float newAngle = math_float_approach_value(currentAngle, desiredAngle, rateFromDelta * approachSpeed);
		int32_t iAngle = (int32_t)newAngle;
		bigpemu_jag_write16(CF_STEER_ADDR, (uint16_t)iAngle);
		sGS.mAnalogSteering = newAngle;
		
		bigpemu_jag_m68k_set_pc(CF_STEER_RET_PC);
	}
}

static void cf_set_speed_bp(const uint32_t addr)
{
	if (sLastDeltaSpeedCor)
	{
		uint32_t curSpeed = bigpemu_jag_read16(CF_SPEED_ADDR + 2) >> 4;
		
		const float rateFromDelta = sGS.mDeltaScale * 2.0f;
		const float finalSpeed = BIGPEMU_MIN((float)curSpeed * rateFromDelta, 32767.0f);
		
		bigpemu_jag_m68k_set_dreg(1, (bigpemu_jag_m68k_get_dreg(1) & 0xFFFF0000) | (uint32_t)finalSpeed);

		bigpemu_jag_m68k_set_pc(CF_SET_SPEED_REENTRY_PC);
	}
}

static void cf_adjust_speed_for_tilt_bp(const uint32_t addr)
{
	if (sLastDeltaSpeedCor)
	{
		uint32_t curSpeed = bigpemu_jag_m68k_get_dreg(0);
		
		const float rateFromDelta = sGS.mDeltaScale * 2.0f;
		const float invScale = (rateFromDelta > 0.0f) ? 1.0f / rateFromDelta : 1.0f;
		const float finalSpeed = (float)curSpeed * invScale;
		
		bigpemu_jag_m68k_set_dreg(0, (uint32_t)finalSpeed);
	}
}

static void cf_camera_swing_bp(const uint32_t addr)
{
	if (sLastAnalogCamSwing)
	{
		if (sGS.mLastAnalogYaw < 0.0f)
		{
			bigpemu_jag_m68k_set_dreg(0, bigpemu_jag_m68k_get_dreg(0) | (1 << JOY_LEFT));
		}
		else if (sGS.mLastAnalogYaw > 0.0f)
		{
			bigpemu_jag_m68k_set_dreg(0, bigpemu_jag_m68k_get_dreg(0) | (1 << JOY_RIGHT));
		}
	}
}

static void cf_tilt_adjust_bp(const uint32_t addr)
{
	//camera tilt stuff is broke-as-hell with uncapped framerate, so just auto-disable it when uncapping. terrible feature, not really worth fixing.
	if (!sLastCamTilt || sLastUncapFramerate)
	{
		bigpemu_jag_write16(CF_CAM_TILT_PARAM0_ADDR, 0);
		bigpemu_jag_write16(CF_CAM_TILT_PARAM1_ADDR, 0);
		bigpemu_jag_write16(CF_CAM_TILT_PARAM2_ADDR, 0);
		bigpemu_jag_write16(CF_CAM_TILT_PARAM3_ADDR, 0);
		bigpemu_jag_write16(CF_CAM_TILT_PARAM4_ADDR, 0);
		bigpemu_jag_write16(CF_CAM_TILT_PARAM5_ADDR, 0);
	}
}

static void cf_looptime_bp(const uint32_t addr)
{
	const double curTime = get_abs_time_ms();
	const float timeDelta = BIGPEMU_MAX(BIGPEMU_MIN((float)(curTime - sGS.mPrevTime), CF_MAX_TIME_DELTA), CF_MIN_TIME_DELTA);
	sGS.mPrevTime = curTime;
	sGS.mCurDelta = timeDelta;
	sGS.mDeltaScale = timeDelta / 50.0f;
#ifdef CF_MONITOR_FRAME_TIMES
	if (sLastUncapFramerate)
	{
		const float minGoodTime = (sLastUncapFramerate > 30) ? 18.0f : 35.0f;
		if (timeDelta > minGoodTime)
		{
			printf("Bad frame time: %f\n", timeDelta);
		}
	}
#endif
}

static void cf_renderloop_bp(const uint32_t addr)
{
	if (sLastUncapFramerate)
	{
		bigpemu_jag_write16(CF_FRAMELIMIT_ADDR, (sLastUncapFramerate > 30) ? 0 : 1);
	}

	//debugging (since there's an option to do this in the main menu), force map display off
	//bigpemu_jag_write8(CF_MAPTYPE_ADDR, 2);
}

static void cf_playsound_bp(const uint32_t addr)
{
	if (!sLastSkidNoise && bigpemu_jag_m68k_get_dreg(0) == CF_SFX_SKIDNOISE_ADDR)
	{
		bigpemu_jag_m68k_set_pc(CF_PLAYSOUND_RET_PC);
	}
}

static void cf_gpu_decomp_bp(const uint32_t addr)
{
	const uint32_t dstAddr = bigpemu_jag_m68k_get_areg(0);
	if (dstAddr == CF_SKY_TEX_ADDR)
	{
		//for byte-style decompression, a1 is the pointer to the compressed data stream.
		//for word-style decompression, a1 is the run data and a2 is the separate stream of word (cry) samples.
		//for our purposes, though, we just want a unique address associated with the sky, so we use a1 in both cases.
		const uint32_t srcAddr = bigpemu_jag_m68k_get_areg(1);
		//this is how the skSkyTextureAddresses list was created
		//printf("Sky source: %08x\n", srcAddr);
		
		sSkyTextureOverrideIndex = -1;
		for (int32_t skyIndex = 0; skyIndex < CF_SKY_TEXTURE_COUNT; ++skyIndex)
		{
			if (skSkyTextureAddresses[skyIndex] == srcAddr)
			{
				const SSkyTextureOverride *pSkyTex = &sSkyTextureOverrides[skyIndex];
				if (pSkyTex->mResPtr)
				{
					//looks like we've got a cusotm texture for the active sky, so set the index
					sSkyTextureOverrideIndex = skyIndex;
				}
				break;
			}
		}
	}
}

static void cf_load_custom_skies()
{
	void *pMod = bigpemu_get_module_handle();
	void *pSkyBuffer = NULL;
	uint32_t skyBufferSize = 0;
	char skyName[BIGP_CRT_MAX_LOCAL_BUFFER_SIZE];
	for (int32_t skyIndex = 0; skyIndex < CF_SKY_TEXTURE_COUNT; ++skyIndex)
	{
		sprintf(skyName, "cf_custom_sky_%08x.png", skSkyTextureAddresses[skyIndex]);
		const uint64_t skyFile = fs_open(skyName, 0);
		if (skyFile)
		{
			const uint32_t fileSize = (uint32_t)fs_get_size(skyFile);
			if (fileSize > skyBufferSize)
			{
				if (pSkyBuffer)
				{
					bigpemu_vm_free(pSkyBuffer);
				}
				//(re)allocate a buffer to read the file if necessary
				pSkyBuffer = bigpemu_vm_alloc(fileSize);
			}
			fs_read(pSkyBuffer, fileSize, skyFile);
			fs_close(skyFile);
			
			uint32_t width, height;
			uint32_t texFlags = BIGPEMU_TEXFLAG_REPEAT;
			if (sLastTexFilter)
			{
				texFlags |= (BIGPEMU_TEXFLAG_BILINEAR | BIGPEMU_TEXFLAG_GENMIPS);
			}
			const TSharedResPtr resPtr = bigpemu_res_texture_from_png(pMod, &width, &height, pSkyBuffer, fileSize, texFlags);
			if (resPtr)
			{
				//alright, we got a custom sky texture, store the dimensions and handle off
				SSkyTextureOverride *pSkyTex = &sSkyTextureOverrides[skyIndex];
				pSkyTex->mWidth = width;
				pSkyTex->mHeight = height;
				pSkyTex->mResPtr = resPtr;
			}
		}
	}
	
	if (pSkyBuffer)
	{
		bigpemu_vm_free(pSkyBuffer);
	}
}

static void cf_free_custom_skies()
{
	void *pMod = bigpemu_get_module_handle();
	for (int32_t skyIndex = 0; skyIndex < CF_SKY_TEXTURE_COUNT; ++skyIndex)
	{
		const SSkyTextureOverride *pSkyTex = &sSkyTextureOverrides[skyIndex];
		if (pSkyTex->mResPtr)
		{
			bigpemu_res_texture_free(pMod, pSkyTex->mResPtr);
		}
	}
	memset(sSkyTextureOverrides, 0, sizeof(sSkyTextureOverrides));
}

static void cf_reset_local_vars()
{
	memset(&sGS, 0, sizeof(sGS));
	sGS.mSaveVersion = CF_SAVE_VERSION;
}

static uint32_t cf_is_loaded()
{
	const uint64_t hash = bigpemu_get_loaded_fnv1a64();
	return (hash == CF_SUPPORTED_HASH) ? 1 : 0;
}

static uint32_t on_sw_loaded(const int32_t eventHandle, void *pEventData)
{
	if (cf_is_loaded())
	{
		cf_reset_local_vars();
		sSkyOp = kSkyOp_Load;
		sGS.mGameLoaded = 1;
		bigpemu_jag_m68k_bp_add(CF_UPLOAD_GPU_DRAW_CODE_PC, cf_upload_draw_code_bp);
		bigpemu_jag_m68k_bp_add(CF_DRAW_SKY_CODE_PC, cf_draw_sky_bp);
		bigpemu_jag_m68k_bp_add(CF_BUFFER_FLIP_PC, cf_buffer_flip_bp);
		bigpemu_jag_m68k_bp_add(CF_GAME_BEGIN_PC, cf_game_begin_bp);
		bigpemu_jag_m68k_bp_add(CF_GAME_END_PC, cf_game_end_bp);
		bigpemu_jag_m68k_bp_add(CF_UPDATECLUT_PC, cf_updateclut_bp);
		bigpemu_jag_m68k_bp_add(CF_CHECK_STEER_INPUT_PC, cf_check_steer_bp);
		bigpemu_jag_m68k_bp_add(CF_SET_SPEED_PC, cf_set_speed_bp);
		bigpemu_jag_m68k_bp_add(CF_ADJUST_SPEED_FOR_TILT_PC, cf_adjust_speed_for_tilt_bp);
		bigpemu_jag_m68k_bp_add(CF_CAMERA_SWING_PC, cf_camera_swing_bp);
		bigpemu_jag_m68k_bp_add(CF_TILT_ADJUST_PC, cf_tilt_adjust_bp);
		bigpemu_jag_m68k_bp_add(CF_STARTLOOP_PC, cf_looptime_bp);
		bigpemu_jag_m68k_bp_add(CF_GAMELOOP_PC, cf_looptime_bp);
		bigpemu_jag_m68k_bp_add(CF_RENDERLOOP_PC, cf_renderloop_bp);
		bigpemu_jag_m68k_bp_add(CF_PLAYSOUND_PC, cf_playsound_bp);
		bigpemu_jag_m68k_bp_add(CF_GPU_DECOMP_BYTE_PC, cf_gpu_decomp_bp);
		bigpemu_jag_m68k_bp_add(CF_GPU_DECOMP_WORD_PC, cf_gpu_decomp_bp);
	}
	else
	{
		sSkyOp = kSkyOp_Free;
		sGS.mGameLoaded = 0;
	}
	return 0;
}

static uint32_t on_video_frame(const int32_t eventHandle, void *pEventData)
{
	if (sGS.mGameLoaded)
	{	
		bigpemu_get_setting_value(&sLastNativeRender, sNativeRenderSettingHandle);
		bigpemu_get_setting_value(&sLastTexFilter, sTexFilterSettingHandle);
		bigpemu_get_setting_value(&sLastUncapFramerate, sUncapFramerateSettingHandle);
		bigpemu_get_setting_value(&sLastAnalogSteering, sAnalogSteeringSettingHandle);
		bigpemu_get_setting_value(&sLastAnalogExponent, sAnalogExponentSettingHandle);
		bigpemu_get_setting_value(&sLastAnalogAccelSpeed, sAnalogAccelSpeedSettingHandle);
		bigpemu_get_setting_value(&sLastAnalogDecelSpeed, sAnalogDecelSpeedSettingHandle);
		bigpemu_get_setting_value(&sLastAnalogConstantLimit, sAnalogConstantLimitSettingHandle);
		bigpemu_get_setting_value(&sLastAnalogLimitScale, sAnalogLimitScaleSettingHandle);
		bigpemu_get_setting_value(&sLastAnalogCamSwing, sAnalogCamSwingSettingHandle);
		bigpemu_get_setting_value(&sLastDeltaSpeedCor, sDeltaSpeedCorSettingHandle);
		bigpemu_get_setting_value(&sLastCamTilt, sCamTiltSettingHandle);
		bigpemu_get_setting_value(&sLastSkidNoise, sSkidNoiseSettingHandle);
		bigpemu_get_setting_value(&sLastDrawRain, sDrawRainSettingHandle);
	}
	if (sSkyOp != kSkyOp_None)
	{
		switch (sSkyOp)
		{
		case kSkyOp_Load:
			cf_free_custom_skies();
			cf_load_custom_skies();
			break;
		case kSkyOp_Free:
			cf_free_custom_skies();
			break;
		default:
			assert(!"invalid sky op");
			break;
		}
		sSkyOp = kSkyOp_None;
	}
	return 0;
}

static uint32_t pre_ui(const int32_t eventHandle, void *pEventData)
{
	float vSize[2];
	float vToN[2];
	bigpemu_drawui_get_virtual_canvas_size(vSize);
	bigpemu_drawui_get_virtual_to_native_scales(vToN);
	sNativeWidth = floorf(vSize[0] * vToN[0] + 0.5f);
	sNativeHeight = floorf(vSize[1] * vToN[1] + 0.5f);
}

static uint32_t on_save_state(const int32_t eventHandle, void *pEventData)
{
	memcpy(pEventData, &sGS, sizeof(sGS));
	return sizeof(sGS);
}

static uint32_t on_load_state(const int32_t eventHandle, void *pEventData)
{
	if (*(int32_t *)pEventData == sGS.mSaveVersion)
	{
		memcpy(&sGS, pEventData, sizeof(sGS));
	}
	return 0;
}

void bigp_init()
{
	void *pMod = bigpemu_get_module_handle();
	
	bigpemu_set_module_usage_flags(pMod, BIGPEMU_MODUSAGE_DETERMINISMWARNING);
	
	const int32_t catHandle = bigpemu_register_setting_category(pMod, "Checkered Flag");
	sNativeRenderSettingHandle = bigpemu_register_setting_bool(catHandle, "Native Res", CF_NATIVE_RES_DEFAULT);
	sTexFilterSettingHandle = bigpemu_register_setting_bool(catHandle, "Texture Filter", CF_TEX_FILTER_DEFAULT);
	sCRYBlendingSettingHandle = bigpemu_register_setting_bool(catHandle, "CRY Blending", CF_CRY_BLENDING_DEFAULT);
	sUncapFramerateSettingHandle = bigpemu_register_setting_int_full(catHandle, "Uncap Framerate", CF_UNCAPFRAMERATE_DEFAULT, 0, 60, 30);	
	sAnalogSteeringSettingHandle = bigpemu_register_setting_bool(catHandle, "Analog Steering", CF_ANALOG_STEERING_DEFAULT);	
	sAnalogExponentSettingHandle = bigpemu_register_setting_float_full(catHandle, "Analog Exponent", CF_ANALOG_EXPONENT_DEFAULT, 0.1f, 32.0f, 0.1f);
	sAnalogAccelSpeedSettingHandle = bigpemu_register_setting_float_full(catHandle, "Analog Accel Speed", CF_ANALOG_ACCEL_SPEED_DEFAULT, 1.0f, CF_STEERING_RANGE * 2.0f, 1.0f);
	sAnalogDecelSpeedSettingHandle = bigpemu_register_setting_float_full(catHandle, "Analog Decel Speed", CF_ANALOG_DECEL_SPEED_DEFAULT, 1.0f, CF_STEERING_RANGE * 2.0f, 1.0f);
	sAnalogConstantLimitSettingHandle = bigpemu_register_setting_float_full(catHandle, "Analog Const Limit", CF_ANALOG_CONSTANT_LIMIT_DEFAULT, 0.0f, CF_STEERING_RANGE * 2.0f, 1.0f);
	sAnalogLimitScaleSettingHandle = bigpemu_register_setting_float_full(catHandle, "Analog Limit Scale", CF_ANALOG_LIMIT_SCALE_DEFAULT, 0.1f, 32.0f, 0.1f);
	sAnalogCamSwingSettingHandle = bigpemu_register_setting_bool(catHandle, "Analog Cam Swing", CF_ANALOG_CAMSWING_DEFAULT);
	sDeltaSpeedCorSettingHandle = bigpemu_register_setting_bool(catHandle, "Delta Speed Comp", CF_DELTASPEEDCOR_DEFAULT);	
	sCamTiltSettingHandle = bigpemu_register_setting_bool(catHandle, "Camera Tilt", CF_CAMTILT_DEFAULT);	
	sSkidNoiseSettingHandle = bigpemu_register_setting_bool(catHandle, "Skid Noise", CF_SKIDNOISE_DEFAULT);	
	sDrawRainSettingHandle = bigpemu_register_setting_bool(catHandle, "Draw Rain", CF_DRAWRAIN_DEFAULT);	
	
	sOnLoadEvent = bigpemu_register_event_sw_loaded(pMod, on_sw_loaded);
	sOnVideoEvent = bigpemu_register_event_video_thread_frame(pMod, on_video_frame);
	sPreUIEvent = bigpemu_register_event_pre_ui(pMod, pre_ui);
	//registering additional events which use different hashes but the same callback is also possible, in case you want to support variants
	sOnStateSaveEvent = bigpemu_register_event_save_state(pMod, on_save_state, CF_SUPPORTED_HASH);
	sOnStateLoadEvent = bigpemu_register_event_load_state(pMod, on_load_state, CF_SUPPORTED_HASH);
	
	cf_reset_local_vars();
	memset(sSkyTextureOverrides, 0, sizeof(sSkyTextureOverrides));
}

void bigp_shutdown()
{
	void *pMod = bigpemu_get_module_handle();
	bigpemu_unregister_event(pMod, sOnLoadEvent);
	bigpemu_unregister_event(pMod, sOnVideoEvent);
	bigpemu_unregister_event(pMod, sPreUIEvent);
	bigpemu_unregister_event(pMod, sOnStateSaveEvent);
	bigpemu_unregister_event(pMod, sOnStateLoadEvent);
	sOnLoadEvent = -1;
	sOnVideoEvent = -1;
	sPreUIEvent = -1;
	sOnStateSaveEvent = -1;
	sOnStateLoadEvent = -1;

	sNativeRenderSettingHandle = -1;
	sTexFilterSettingHandle = -1;
	sCRYBlendingSettingHandle = -1;
	sUncapFramerateSettingHandle = -1;
	sAnalogSteeringSettingHandle = -1;
	sAnalogExponentSettingHandle = -1;
	sAnalogDecelSpeedSettingHandle = -1;
	sAnalogAccelSpeedSettingHandle = -1;
	sAnalogConstantLimitSettingHandle = -1;
	sAnalogLimitScaleSettingHandle = -1;
	sAnalogCamSwingSettingHandle = -1;
	sDeltaSpeedCorSettingHandle = -1;
	sCamTiltSettingHandle = -1;
	sSkidNoiseSettingHandle = -1;
	sDrawRainSettingHandle = -1;
	sLastNativeRender = CF_NATIVE_RES_DEFAULT;
	sLastTexFilter = CF_TEX_FILTER_DEFAULT;
	sLastCRYBlending = CF_CRY_BLENDING_DEFAULT;
	sLastUncapFramerate = CF_UNCAPFRAMERATE_DEFAULT;
	sLastAnalogSteering = CF_ANALOG_STEERING_DEFAULT;
	sLastAnalogExponent = CF_ANALOG_EXPONENT_DEFAULT;
	sLastAnalogDecelSpeed = CF_ANALOG_DECEL_SPEED_DEFAULT;
	sLastAnalogAccelSpeed = CF_ANALOG_ACCEL_SPEED_DEFAULT;
	sLastAnalogConstantLimit = CF_ANALOG_CONSTANT_LIMIT_DEFAULT;
	sLastAnalogLimitScale = CF_ANALOG_LIMIT_SCALE_DEFAULT;
	sLastAnalogCamSwing = CF_ANALOG_CAMSWING_DEFAULT;
	sLastDeltaSpeedCor = CF_DELTASPEEDCOR_DEFAULT;
	sLastCamTilt = CF_CAMTILT_DEFAULT;
	sLastSkidNoise = CF_SKIDNOISE_DEFAULT;
	sLastDrawRain = CF_DRAWRAIN_DEFAULT;
	
	cf_reset_local_vars();
	cf_free_custom_skies();
}
