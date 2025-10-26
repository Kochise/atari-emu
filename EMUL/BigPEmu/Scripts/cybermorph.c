//__BIGPEMU_SCRIPT_MODULE__
//__BIGPEMU_META_DESC__		"Cybermorph enhancements."
//__BIGPEMU_META_AUTHOR__	"Rich Whitehouse"

#include "bigpcrt.h"
#include "jagregs.h"

static int sOnLoadEvent = -1;
static int sOnVideoEvent = -1;

static int sNativeRenderSettingHandle = -1;
static int sTexFilterSettingHandle = -1;

static int sLastNativeRender = 1;
static int sLastTexFilter = 1;

static uint32_t sCurrentUploadPrgAddr = 0;
static uint32_t sCurrentBufferAddr = 0;

#define CM_SUPPORTED_HASH					0x3F97A08E8550667CULL
#define CM_REVB_HASH						0xB16C980E3FB809BBULL
#define CM_UPLOAD_GPU_PC					0x008095E2
#define CM_UPLOAD_GPU_FINISH_PC				0x0080961A
#define CM_POLY_DRAW_GPU_PROG_ADDR			0x008AC038
#define CM_SPRITE_DRAW_GPU_PROG_ADDR		0x008AE7F0
#define CM_GPU_DRAW_POLY_PC					0x00F03524
#define CM_GPU_FINISH_POLY_PC				0x00F038F6
#define CM_GPU_DRAW_SPRITE_PC				0x00F030F0
#define CM_GPU_FINISH_SPRITE_PC				0x00F031D6
#define CM_BG_BLIT_PC						0x00809118
#define CM_FRAMEBUFFER_BASE_ADDR			0x00110000
#define CM_ENDFRAME_PC						0x0080274A
#define CM_FADECOUNT_ADDR					0x00009A50

#define CM_NPOINTS_ADDR						0x00F0392C
#define CM_POLYPOINTS_ADDR					0x00F03930

#define CM_POINT_SIZE 			(4 * 4)
#define CM_POINT_UV_SIZE 		(6 * 4)
#define CM_MAX_POINT_COUNT		256

#define CM_VERT(pVert, x, y, z, c) \
	pVert[0] = byteswap_ulong(x); \
	pVert[1] = byteswap_ulong(y); \
	pVert[2] = byteswap_ulong(z); \
	pVert[3] = c;
#define CM_VERT_UV(pVert, x, y, z, c, u, v) \
	pVert[0] = byteswap_ulong(x); \
	pVert[1] = byteswap_ulong(y); \
	pVert[2] = byteswap_ulong(z); \
	pVert[3] = c; \
	((float *)pVert)[4] = u; \
	((float *)pVert)[5] = v;

static uint8_t sPointBuffer[CM_MAX_POINT_COUNT * CM_POINT_UV_SIZE];

static void cm_upload_gpu_bp(const uint32_t addr)
{
	sCurrentUploadPrgAddr = bigpemu_jag_m68k_get_dreg(0);
}

static void cm_draw_poly_gpubp(const uint32_t addr)
{	
	if (sCurrentBufferAddr)
	{
		//we don't need to worry about data validation, because we patched this breakpoint into the code itself as it was uploaded from the m68k.
		const uint32_t pointCount = bigpemu_jag_read32(CM_NPOINTS_ADDR);
		assert(pointCount < CM_MAX_POINT_COUNT);

		bigpemu_jag_sysmemread(sPointBuffer, CM_POLYPOINTS_ADDR, CM_POINT_SIZE * pointCount);

		SOPVMPolyInfo polyInfo;
		memset(&polyInfo, 0, sizeof(polyInfo));
		polyInfo.mpData = (vmuptr_t)sPointBuffer;
		polyInfo.mVersion = OPVM_POLYINFO_VERSION;
		polyInfo.mPointCount = pointCount;
		polyInfo.mStride = CM_POINT_SIZE;
		polyInfo.mPosOffset = 0;
		polyInfo.mUvOffset = -1;
		polyInfo.mColorOffset = 4 * 3 + 2;
		polyInfo.mTexRefIndex = -1;
		polyInfo.mPosType = kOPVMPos_XyzInt32;
		polyInfo.mColorType = kOPVMColor_CRY16;
		polyInfo.mBlendMode = kSpecialPolyBlend_None;
		
		bigpemu_jag_op_add_poly(sCurrentBufferAddr, &polyInfo);

		bigpemu_jag_gpu_set_pc(CM_GPU_FINISH_POLY_PC);	
	}
	else
	{
		//resume normally
		bigpemu_jag_gpu_curbank_set_reg(8, 0xFFFFFFFF);
		bigpemu_jag_gpu_curbank_set_reg(9, 0xFFFFFFFF);
		bigpemu_jag_gpu_set_pc(CM_GPU_DRAW_POLY_PC + 8);
	}
}

//we could cheat and just render the original poly depths to keep sprites non-native and clipping correctly.
//but instead, we're going whole-hog and rendering sprites at native res too.
static void cm_draw_sprite_gpubp(const uint32_t addr)
{
	if (sCurrentBufferAddr)
	{
		const uint32_t sprWidth = bigpemu_jag_gpu_curbank_get_reg(16);
		const uint32_t sprHeight = bigpemu_jag_gpu_curbank_get_reg(17);
		int32_t sprPos[3] =
		{
			bigpemu_jag_gpu_curbank_get_reg(11),
			bigpemu_jag_gpu_curbank_get_reg(12),
			bigpemu_jag_gpu_curbank_get_reg(10)
		};
		const uint32_t sprCFlags = bigpemu_jag_gpu_curbank_get_reg(22);
		//we're just being lazy making assumptions here (maybe poorly)
		//const uint32_t sprIFlags = bigpemu_jag_gpu_curbank_get_reg(20) | PIXEL16 | XADDINC;
		const uint32_t imgAddr = bigpemu_jag_gpu_curbank_get_reg(19);
		
		const uint32_t origWidth = bigpemu_jag_read16(imgAddr - 16);
		const uint32_t origHeight = bigpemu_jag_read16(imgAddr - 14);

		int32_t clipPos[2];
		int32_t clipSize[2];
		float uvRect[8];
		
		if (!(sprCFlags & 1))
		{
			sprPos[0] -= (sprWidth >> 1);
			sprPos[1] -= (sprHeight >> 1);
		}
	
		if (math_clip_axially_aligned_quad_to_canvas(clipPos, clipSize, sprPos[0], sprPos[1], sprWidth, sprHeight, 320 - 1, 200))
		{
			uint32_t *pVerts = (uint32_t *)sPointBuffer;
			SOPVMPolyInfo polyInfo;
			const uint32_t sprColor = 0xFFFFFFFF;
			const uint32_t dcompVal = 0;
			const uint32_t texFlags = (sLastTexFilter) ? OPVM_TEXINFO_FLAG_WANTFILTER : 0;
			
			const float uvScaleX = 1.0f / (float)sprWidth;
			const float uvScaleY = 1.0f / (float)sprHeight;
			
			uvRect[0] = (float)(clipPos[0] - sprPos[0]) * uvScaleX;
			uvRect[1] = (float)(clipPos[1] - sprPos[1]) * uvScaleY;
			
			uvRect[2] = (float)((clipPos[0] + clipSize[0]) - sprPos[0]) * uvScaleX;
			uvRect[3] = (float)(clipPos[1] - sprPos[1]) * uvScaleY;
			
			uvRect[4] = (float)((clipPos[0] + clipSize[0]) - sprPos[0]) * uvScaleX;
			uvRect[5] = (float)((clipPos[1] + clipSize[1]) - sprPos[1]) * uvScaleY;
			
			uvRect[6] = (float)(clipPos[0] - sprPos[0]) * uvScaleX;
			uvRect[7] = (float)((clipPos[1] + clipSize[1]) - sprPos[1]) * uvScaleY;
			
			memset(&polyInfo, 0, sizeof(polyInfo));
			polyInfo.mpData = (vmuptr_t)sPointBuffer;
			polyInfo.mVersion = OPVM_POLYINFO_VERSION;
			polyInfo.mPointCount = 4;
			polyInfo.mStride = CM_POINT_UV_SIZE;
			polyInfo.mPosOffset = 0;
			polyInfo.mUvOffset = 4 * 4;
			polyInfo.mColorOffset = 4 * 3;
			//we could potentially cache textures for the frame based on address/props, but for now we're just making a new one for every sprite
			polyInfo.mTexRefIndex = bigpemu_jag_op_create_frame_tex(sCurrentBufferAddr, imgAddr, 1, origWidth, origHeight, (origWidth << 1), kOPVMTex_CRY16, texFlags, &dcompVal);
			polyInfo.mPosType = kOPVMPos_XyzInt32;
			polyInfo.mUvType = kOPVMUv_Float32;
			polyInfo.mColorType = kOPVMColor_RGB32;
			polyInfo.mBlendMode = kSpecialPolyBlend_None;
			
			CM_VERT_UV(pVerts, clipPos[0], clipPos[1], sprPos[2], sprColor, uvRect[0], uvRect[1]);
			pVerts += 6;
			CM_VERT_UV(pVerts, clipPos[0] + clipSize[0], clipPos[1], sprPos[2], sprColor, uvRect[2], uvRect[3]);
			pVerts += 6;
			CM_VERT_UV(pVerts, clipPos[0] + clipSize[0], clipPos[1] + clipSize[1], sprPos[2], sprColor, uvRect[4], uvRect[5]);
			pVerts += 6;
			CM_VERT_UV(pVerts, clipPos[0], clipPos[1] + clipSize[1], sprPos[2], sprColor, uvRect[6], uvRect[7]);
			
			bigpemu_jag_op_add_poly(sCurrentBufferAddr, &polyInfo);
		}
		
		bigpemu_jag_gpu_set_pc(bigpemu_jag_gpu_curbank_get_reg(29));
	}
	else
	{
		//replace the relevant logic for the ops we stomped
		//	shlq #8,r15
		//	move r16,r0
		//	move r17,r1
		//	shrq #1,r0
		bigpemu_jag_gpu_curbank_set_reg(15, bigpemu_jag_gpu_curbank_get_reg(15) << 8);
		bigpemu_jag_gpu_curbank_set_reg(0, bigpemu_jag_gpu_curbank_get_reg(16));
		bigpemu_jag_gpu_curbank_set_reg(1, bigpemu_jag_gpu_curbank_get_reg(17));
		bigpemu_jag_gpu_curbank_set_reg(0, bigpemu_jag_gpu_curbank_get_reg(0) >> 1);
	}
}

static void cm_upload_gpu_finish_bp(const uint32_t addr)
{
	if (!sCurrentBufferAddr || !sLastNativeRender)
	{
		return;
	}
	
	if (sCurrentUploadPrgAddr == CM_POLY_DRAW_GPU_PROG_ADDR)
	{
		bigpemu_jag_inject_risc_bp(CM_GPU_DRAW_POLY_PC, cm_draw_poly_gpubp);
	}
	else if (sCurrentUploadPrgAddr == CM_SPRITE_DRAW_GPU_PROG_ADDR)
	{
		bigpemu_jag_inject_risc_bp(CM_GPU_DRAW_SPRITE_PC, cm_draw_sprite_gpubp);
	}
}

static void cm_bg_blit_bp(const uint32_t addr)
{	
	const uint32_t a0 = bigpemu_jag_m68k_get_areg(0);
	const uint32_t bgAddr = bigpemu_jag_read32(a0);
	if (bgAddr >= CM_FRAMEBUFFER_BASE_ADDR && bgAddr <= (CM_FRAMEBUFFER_BASE_ADDR + 0x10))
	{ //make sure we're really targeting the framebuffer
		//polygons are attached to OP objects via their base address, ensuring proper synchronization.
		//clear the poly list on this object now that we're about to start rendering to it again.
		bigpemu_jag_op_clear_buffers(bgAddr);
		if (sLastNativeRender && bigpemu_jag_read16(CM_FADECOUNT_ADDR) == 0)
		{
			sCurrentBufferAddr = bgAddr;
						
			bigpemu_jag_op_set_alpha_fill(1); //make sure alpha is set when converting to rgba32, regardless of pixel mode.
			bigpemu_jag_op_set_special_transparency(bgAddr, 0x80FF, kOPVMColor_CRY16);
			
			//we'll use these to generate a nice native-res interpolated quad
			const int32_t intendedInc = (((int32_t)bigpemu_jag_blitter_raw_get(kBPE_BlitterRaw_IINC) << 8) >> 8);
			const uint32_t intendedPadD0 = bigpemu_jag_blitter_raw_get(kBPE_BlitterRaw_PATD0);
			const uint32_t gourC = ((intendedPadD0 & 0xFFFF) << 8);
						
			//add a poly for the background using the provided gradient values.
			//this thing will be linearly interpolated in rgb space, so it's not quite right, but good enough.
			{
				uint32_t *pVerts = (uint32_t *)sPointBuffer;
				SOPVMPolyInfo polyInfo;
				const uint32_t colorTop = byteswap_ulong(gourC >> 8);
				const uint32_t colorBottom = byteswap_ulong((gourC + intendedInc * 200) >> 8);
				
				memset(&polyInfo, 0, sizeof(polyInfo));
				polyInfo.mpData = (vmuptr_t)sPointBuffer;
				polyInfo.mVersion = OPVM_POLYINFO_VERSION;
				polyInfo.mPointCount = 4;
				polyInfo.mStride = CM_POINT_SIZE;
				polyInfo.mPosOffset = 0;
				polyInfo.mUvOffset = -1;
				polyInfo.mColorOffset = 4 * 3 + 2;
				polyInfo.mTexRefIndex = -1;
				polyInfo.mPosType = kOPVMPos_XyzInt32;
				polyInfo.mColorType = kOPVMColor_CRY16;
				polyInfo.mBlendMode = kSpecialPolyBlend_None;
				
				CM_VERT(pVerts, 0, 0, 65534, colorTop);
				pVerts += 4;
				CM_VERT(pVerts, 320 - 1, 0, 65534, colorTop);
				pVerts += 4;
				CM_VERT(pVerts, 320 - 1, 200, 65534, colorBottom);
				pVerts += 4;
				CM_VERT(pVerts, 0, 200, 65534, colorBottom);
				
				bigpemu_jag_op_add_poly(bgAddr, &polyInfo);
			}
			
			//remove gradient from background, make it a terrible color that we can use to mask (super hack - we'd deal with this at the OP level, but HUD is drawn into this same object)
			bigpemu_jag_write32(B_IINC, 0);
			bigpemu_jag_write32(B_PATD, 0x80FF80FF);
			bigpemu_jag_write32(B_PATD + 4, 0x80FF80FF);
			bigpemu_jag_write32(B_SRCD, 0x80FF80FF);
			bigpemu_jag_write32(B_SRCD + 4, 0x80FF80FF);
		}
		else
		{
			//disable our rendering hooks during transitions
			sCurrentBufferAddr = 0;
		}
	}
}

static void cm_end_frame_bp(const uint32_t addr)
{
	sCurrentBufferAddr = 0;
}

static uint32_t on_sw_loaded(const int eventHandle, void *pEventData)
{
	const uint64_t hash = bigpemu_get_loaded_fnv1a64();
	if (hash == CM_SUPPORTED_HASH)
	{
		sCurrentBufferAddr = 0;
		
		bigpemu_jag_m68k_bp_add(CM_UPLOAD_GPU_PC, cm_upload_gpu_bp);
		bigpemu_jag_m68k_bp_add(CM_UPLOAD_GPU_FINISH_PC, cm_upload_gpu_finish_bp);
		bigpemu_jag_m68k_bp_add(CM_BG_BLIT_PC, cm_bg_blit_bp);
		bigpemu_jag_m68k_bp_add(CM_ENDFRAME_PC, cm_end_frame_bp);
	}
	else if (hash == CM_REVB_HASH)
	{
		printf_notify("This version of Cybermorph is not supported by the active script.");
	}
	return 0;
}

static uint32_t on_video_frame(const int eventHandle, void *pEventData)
{
	bigpemu_get_setting_value(&sLastNativeRender, sNativeRenderSettingHandle);
	bigpemu_get_setting_value(&sLastTexFilter, sTexFilterSettingHandle);
	return 0;
}

void bigp_init()
{
	void *pMod = bigpemu_get_module_handle();
	
	bigpemu_set_module_usage_flags(pMod, BIGPEMU_MODUSAGE_DETERMINISMWARNING);
	
	const int catHandle = bigpemu_register_setting_category(pMod, "Cybermorph");
	sNativeRenderSettingHandle = bigpemu_register_setting_bool(catHandle, "Native Res", 1);
	sTexFilterSettingHandle = bigpemu_register_setting_bool(catHandle, "Texture Filter", 1);
	
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
	sNativeRenderSettingHandle = -1;
	sTexFilterSettingHandle = -1;
	sLastNativeRender = 1;
	sLastTexFilter = 1;
	sCurrentUploadPrgAddr = 0;
	sCurrentBufferAddr = 0;
}
