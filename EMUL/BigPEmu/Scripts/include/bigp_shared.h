#pragma once
#ifndef _BIGP_SHARED_H
#define _BIGP_SHARED_H

#define BIGPEMU_VM_SYSCALL_ADDR 0xFFFFFFFF

#define BIGPEMU_VM_COMPILED_VERSION 2

typedef uint32_t vmuptr_t;
typedef uint64_t TSharedResPtr;

typedef enum
{
	kBPESys_Print = 0,
	kBPESys_NotifyText,
	kBPESys_FailedAssert,
	
	kBPESys_ScriptAPIVersion,
	
	kBPESys_Platform,
	kBPESys_TimeMS,
	
	kBPE_VMAlloc,
	kBPE_VMFree,
	
	kBPE_NativeMod_Load,
	kBPE_NativeMod_Free,
	kBPE_NativeMod_GetFuncAddr,
	kBPE_ProcessMemoryAlias,
	kBPE_PrepareForNativeCall,
	
	kBPE_Sin,
	kBPE_Cos,
	kBPE_Tan,
	kBPE_ASin,
	kBPE_ACos,
	kBPE_ATan,
	kBPE_ATan2,
	kBPE_Exp,
	kBPE_FMod,
	kBPE_Ceil,
	kBPE_Floor,
	kBPE_Log,
	kBPE_Pow,
	kBPE_FAbs,
	kBPE_Sqrt,
	
	kBPE_DoubleToString,
	kBPE_StringToDouble,
	kBPE_StringToInt,
	kBPE_C9XStrToUL,
	kBPE_C9XStrToL,
	kBPE_C9XStrToD,
	
	kBPE_StrCpy,
	kBPE_StrNCpy,
	kBPE_StrLen,
	kBPE_StrCat,
	kBPE_StrCmp,
	kBPE_StrICmp,
	kBPE_StrNICmp,
	kBPE_StrStr,
	kBPE_CToUpper,
	kBPE_CToLower,
	
	kBPE_MemCpy,
	kBPE_MemSet,
	kBPE_MemCmp,
	
	kBPE_RegisterEvent_SWLoaded,
	kBPE_RegisterEvent_SWUnloaded,
	kBPE_RegisterEvent_EmuFrame,
	kBPE_RegisterEvent_VideoFrame,
	kBPE_UnregisterEvent,
	
	kBPE_GetLoadedFNV1a,
	kBPE_GetLoadedSoftwarePath,
	
	kBPE_RegisterSettingCat,
	kBPE_RegisterSetting,	
	kBPE_GetSettingValue,
	
	kBPE_FS_Open,
	kBPE_FS_Read,
	kBPE_FS_Write,
	kBPE_FS_Close,
	
	kBPE_Jag_Read8,
	kBPE_Jag_Read16,
	kBPE_Jag_Read32,
	kBPE_Jag_Read64,
	kBPE_Jag_Write8,
	kBPE_Jag_Write16,
	kBPE_Jag_Write32,
	kBPE_Jag_Write64,
	
	kBPE_Jag_SysMemCmp,
	kBPE_Jag_SysMemSet,
	kBPE_Jag_SysMemRead,
	kBPE_Jag_SysMemWrite,
	
	kBPE_Jag_M68K_BP_Add,
	kBPE_Jag_M68K_BP_Del,
	
	kBPE_Jag_M68K_SetPC,
	kBPE_Jag_M68K_GetPC,
	kBPE_Jag_M68K_SetDReg,
	kBPE_Jag_M68K_GetDReg,
	kBPE_Jag_M68K_SetAReg,
	kBPE_Jag_M68K_GetAReg,
	
	kBPE_Jag_GPU_SetPC,
	kBPE_Jag_GPU_GetPC,
	kBPE_Jag_GPU_SetReg,
	kBPE_Jag_GPU_GetReg,
	kBPE_Jag_GPU_CurBank_SetReg,
	kBPE_Jag_GPU_CurBank_GetReg,
	kBPE_Jag_GPU_AltBank_SetReg,
	kBPE_Jag_GPU_AltBank_GetReg,

	kBPE_Jag_DSP_SetPC,
	kBPE_Jag_DSP_GetPC,
	kBPE_Jag_DSP_SetReg,
	kBPE_Jag_DSP_GetReg,
	kBPE_Jag_DSP_CurBank_SetReg,
	kBPE_Jag_DSP_CurBank_GetReg,
	kBPE_Jag_DSP_AltBank_SetReg,
	kBPE_Jag_DSP_AltBank_GetReg,
	
	kBPE_Jag_SetSysFlags,
	kBPE_Jag_GetSysFlags,
	kBPE_Jag_SetM68KClockScale,
	kBPE_Jag_SetRISCClockScale,
	kBPE_Jag_SetButchSeekScale,
	kBPE_Jag_SetLockCycles,
	
	kBPE_Jag_Jerry_SetIntEn,
	kBPE_Jag_Jerry_GetIntEn,
	kBPE_Jag_Jerry_SetIntPend,
	kBPE_Jag_Jerry_GetIntPend,

	kBPE_Jag_Tom_SetIntEn,
	kBPE_Jag_Tom_GetIntEn,
	kBPE_Jag_Tom_SetIntPend,
	kBPE_Jag_Tom_GetIntPend,
	kBPE_Jag_Tom_SetBlitterExCycles,
	
	kBPE_Jag_Blitter_RawSet,
	kBPE_Jag_Blitter_RawGet,
	
	kBPE_Jag_OP_SetSpecialTransparency,
	kBPE_Jag_OP_AddPoly,
	kBPE_Jag_OP_ClearPolyBuffer,
	kBPE_Jag_OP_SetAlphaFill,
	kBPE_Jag_OP_CreateFrameTex,
	
	kBPE_Jag_IsNTSC,
	kBPE_Jag_GetFrameCount,
	kBPE_Jag_GetLineCount,
	kBPE_Jag_GetExecTime,
	kBPE_Jag_GetHorizontalPeriod,
	kBPE_Jag_GetFramePeriod,
	kBPE_Jag_GetMasterClockMHz,
	kBPE_Jag_RISCCycleForUSec,
	kBPE_Jag_M68KCycleForUSec,
	kBPE_Jag_USecForRISCCycle,
	kBPE_Jag_USecForM68KCycle,
	
	kBPE_Jag_M68K_ConsumeCycles,
	kBPE_Jag_GPU_ConsumeCycles,
	kBPE_Jag_DSP_ConsumeCycles,

	kBPE_Jag_GPU_SetPipelineEnabled,
	kBPE_Jag_DSP_SetPipelineEnabled,
	
	kBPE_Jag_InjectRISCBP,
	
	kBPE_Jag_GetDeviceType,
	
	kBPE_Jag_SetTrackerConstraintsX,
	kBPE_Jag_SetTrackerConstraintsY,
	
	kBPE_Jag_SetStereoEnabled,
	kBPE_Jag_SetStereoScanEye,
	
	kBPE_AtomicFToI,
	kBPE_AtomicDToI,
	
	kBPE_Jag_SetPaused,
	
	kBPE_Math_Vec3Copy,
	kBPE_Math_Vec3Add,
	kBPE_Math_Vec3Add2,
	kBPE_Math_Vec3Sub,
	kBPE_Math_Vec3Sub2,
	kBPE_Math_Vec3Scale,
	kBPE_Math_Vec3Scale2,
	kBPE_Math_Vec3ScaleF,
	kBPE_Math_Vec3AddScaled,
	kBPE_Math_Vec3Dot,
	kBPE_Math_Vec3LengthSq,
	kBPE_Math_Vec3Length,
	kBPE_Math_Vec3Normalize,
	kBPE_Math_Vec3Cross,
	kBPE_Math_Vec3ClampF,
	
	kBPE_Math_Mat44Identity,
	kBPE_Math_Mat44Multiply,
	kBPE_Math_Mat44Inverse,
	kBPE_Math_Mat44Rotate,
	kBPE_Math_Mat44Translate,
	kBPE_Math_Mat44TransformVec3,
	kBPE_Math_Mat44TransformVec4,
	
	kBPE_FS_GetSize,
	
	kBPE_Input_GetInputSize,
	kBPE_Input_GetAllHeldInputs,
	kBPE_Input_CreateInputFromVK,
	kBPE_Input_InputInSet,
	kBPE_Input_GetInputName,
	
	kBPE_RegisterEvent_PreUI,
	kBPE_RegisterEvent_PostUI,
	
	kBPE_Res_TextureFromPNG,
	kBPE_Res_TextureFromRGBA32,
	kBPE_Res_TextureFree,
	
	kBPE_Res_SoundFromWave,
	kBPE_Res_SoundFree,
	
	kBPE_DrawUI_GetVirtualWidthAndHeight,
	kBPE_DrawUI_GetVirtualToNativeScales,
	kBPE_DrawUI_Text,
	kBPE_DrawUI_TextBounds,
	kBPE_DrawUI_Rect,
	kBPE_DrawUI_OutlinedRect,
	kBPE_DrawUI_Lines,
	
	kBPE_Audio_PlaySound,
	
	kBPE_RegisterEvent_NetUpdate,
	kBPE_RegisterEvent_NetReceive,
	kBPE_Net_CurrentDevice,
	kBPE_Net_ConnectionType,
	kBPE_Net_ClientIndex,
	kBPE_Net_ClientSWHash,
	kBPE_Net_Send,
	kBPE_Net_SendElems,
	kBPE_Net_SendNoDelta,
	kBPE_Net_Recv,
	kBPE_Net_Behind,
	
	kBPE_GetROMDirList,
	kBPE_LoadROMFromCurrentDir,
	
	kBPE_Net_HostPrint,
	kBPE_Net_Disconnect,
	kBPE_Net_LastClient,
	
	kBPE_Jag_GetInputButtons,
	kBPE_Jag_GetInputExButtons,
	kBPE_Jag_GetInputAnalogs,
	
	kBPE_Math_ClipAxiallyAlignedQuadToCanvas,
	kBPE_Math_Clip2DPolygon,
	
	kBPE_Jag_OP_EnablePlayAreaScissor,
	
	kBPE_RegisterEvent_SaveState,
	kBPE_RegisterEvent_LoadState,
	
	kBPE_Jag_OP_RenderBitmapObjectToBuffer,
	
	kBPE_TestData,
	
	kBPESys_Count
} EBigPEmuSysCall;

typedef enum
{
	kBPE_Platform_Win64 = 0,
	kBPE_Platform_Linux64,
	kBPE_Platform_LinuxArm32,
	kBPE_Platform_LinuxArm64,
	kBPE_Platform_Switch,
	kBPE_Platform_PS4,
	kBPE_Platform_PS5,
	kBPE_Platform_XB1,
	kBPE_Platform_XBSX,
	kBPE_Platform_Other,
	
	kBPE_Platform_Count
} EBigPEmuPlatform;

typedef enum
{
	kBPE_BlitterRaw_A1BASE,
	kBPE_BlitterRaw_A1FLAGS,
	kBPE_BlitterRaw_A1CLIP,
	kBPE_BlitterRaw_A1PIXEL,
	kBPE_BlitterRaw_A1STEP,
	kBPE_BlitterRaw_A1FSTEP,
	kBPE_BlitterRaw_A1FPIXEL,
	kBPE_BlitterRaw_A1IINC,
	kBPE_BlitterRaw_A1FINC,
	kBPE_BlitterRaw_A2BASE,
	kBPE_BlitterRaw_A2FLAGS,
	kBPE_BlitterRaw_A2MASK,
	kBPE_BlitterRaw_A2PIXEL,
	kBPE_BlitterRaw_A2STEP,
	kBPE_BlitterRaw_B_CMD,
	kBPE_BlitterRaw_COUNT,
	kBPE_BlitterRaw_SRCD0,
	kBPE_BlitterRaw_SRCD1,
	kBPE_BlitterRaw_DSTD0,
	kBPE_BlitterRaw_DSTD1,
	kBPE_BlitterRaw_DSTZ0,
	kBPE_BlitterRaw_DSTZ1,
	kBPE_BlitterRaw_SRCZ10,
	kBPE_BlitterRaw_SRCZ11,
	kBPE_BlitterRaw_SRCZ20,
	kBPE_BlitterRaw_SRCZ21,
	kBPE_BlitterRaw_PATD0,
	kBPE_BlitterRaw_PATD1,
	kBPE_BlitterRaw_IINC,
	kBPE_BlitterRaw_ZINC,
	
	kBPE_BlitterRaw_Count
} EBigPEmuBlitterRaw;

typedef enum
{
	kOPVMPos_XyzInt32 = 0, //big-endian
	kOPVMPos_XyzFloat32 //little-endian (assume script-generated)
} EOPVMPosType;

typedef enum
{
	kOPVMUv_Float32 = 0 //little-endian
} EOPVMUvType;

typedef enum
{
	kOPVMColor_CRY16 = 0, //big-endian
	kOPVMColor_RGB16, //big-endian
	kOPVMColor_RGB32 //big-endian
} EOPVMColorType;

typedef enum
{
	kOPVMTex_CRY16 = 0 //big-endian
} EOPVMTexType;

typedef enum
{
	kSpecialPolyBlend_None = 0,
	kSpecialPolyBlend_Alpha,
	kSpecialPolyBlend_Additive
} EOPVMBlendMode;

#define OPVM_POLYINFO_VERSION 666 //to maintain compiled/binary vm compatibility across struct changes

#define OPVM_POLYINFO_FLAG_NODEPTHTEST	(1 << 0)
#define OPVM_POLYINFO_FLAG_NODEPTHWRITE	(1 << 1)
#define OPVM_POLYINFO_FLAG_ADDDSEL		(1 << 2)
#define OPVM_POLYINFO_FLAG_ADDDSEL_SIGN	(1 << 3)
#define OPVM_POLYINFO_FLAG_KEEPCRY		(1 << 4)
typedef struct
{
	vmuptr_t mpData;
	uint32_t mVersion;
	uint32_t mPointCount;
	uint32_t mStride;
	uint32_t mFlags;
	int32_t mPosOffset;
	int32_t mUvOffset;
	int32_t mColorOffset;
	int32_t mTexRefIndex;
	EOPVMPosType mPosType;
	EOPVMUvType mUvType;
	EOPVMColorType mColorType;
	EOPVMBlendMode mBlendMode;
	uint32_t mResv0;
} SOPVMPolyInfo;

#define OPVM_TEXINFO_FLAG_USEDCOMPEN	(1 << 0)
#define OPVM_TEXINFO_FLAG_WANTFILTER	(1 << 1)
#define OPVM_TEXINFO_FLAG_KEEPCRY		(1 << 2)
typedef struct
{
	uint32_t mAddr;
	uint32_t mIsJagAddr;
	uint32_t mWidth;
	uint32_t mHeight;
	uint32_t mPitch;
	uint32_t mDCompEnVal;
	uint32_t mFlags;
	EOPVMTexType mTexType;
	TSharedResPtr mExtTexture;	
} SOPVMTexInfo;

typedef enum
{
	kBPE_DT_Standard = 0,
	kBPE_DT_Rotary,
	kBPE_DT_Analog,
	kBPE_DT_Driving,
	kBPE_DT_AnalogADC,
	kBPE_DT_HeadTracker,
	kBPE_DT_Count
} ESharedEmulatorDeviceType;

typedef enum
{
	kBPE_VMSetting_Bool = 0,
	kBPE_VMSetting_Int,
	kBPE_VMSetting_Float,

	kBPE_VMSetting_Count
} EBigPEmuSettingType;

typedef struct
{
	int32_t mCatHandle;
	uint32_t mType;
	vmuptr_t mModAddr;
	vmuptr_t mNameAddr;
	vmuptr_t mDefaultValAddr;
	vmuptr_t mMinValAddr;
	vmuptr_t mMaxValAddr;
	vmuptr_t mStepValAddr;
} TBigPEmuNewSettingParams;

typedef struct
{
	uint64_t mMod;
	uint64_t mFuncAddr;
	uint64_t mEngine0;
	uint64_t mEngine1;
} TNativeCallInfo;

typedef enum
{
	kDrawUI_TJ_Left = 0,
	kDrawUI_TJ_Right,
	kDrawUI_TJ_Center
} EDrawUITextJustify;

typedef struct
{
	float mX;
	float mY;
	float mScale;
	vmuptr_t mpText;
	vmuptr_t mpRgba;
	uint32_t mTJ; //EDrawUITextJustify
	float mWrapDist;
	uint32_t mResv0;
} TDrawUITextParams;

typedef struct
{
	vmuptr_t mpOut;
	vmuptr_t mpText;
	float mScale;
	float mWrapDist;
	uint32_t mResv0;
	uint32_t mResv1;
} TDrawUITextBoundsParams;

typedef struct
{
	TSharedResPtr mpTexture;
	float mX;
	float mY;
	float mWidth;
	float mHeight;
	vmuptr_t mpRgba;
	vmuptr_t mpSecRgba;
	uint32_t mResv0;
	uint32_t mResv1;
} TDrawUIRectParams;

typedef struct
{
	float mX;
	float mY;
	float mWidth;
	float mHeight;
	vmuptr_t mpRgba;
	vmuptr_t mpSecRgba;
	float mBorderWidth;
	vmuptr_t mpBorderRgba;
	uint32_t mResv0;
	uint32_t mResv1;
} TDrawUIOutlinedRectParams;

typedef struct
{
	vmuptr_t mpPoints;
	uint32_t mPointCount;
	float mWidth;
	vmuptr_t mpRgba;
	float mInnerWidth;
	float mHardness;
	vmuptr_t mpSecRgba;
	float mAttnDist;
	uint32_t mResv0;
	uint32_t mResv1;
} TDrawUILinesParams;

#define BIGPEMU_TEXFLAG_NONE				0
#define BIGPEMU_TEXFLAG_BILINEAR			(1 << 0)
#define BIGPEMU_TEXFLAG_REPEAT				(1 << 1)
#define BIGPEMU_TEXFLAG_CLAMP_TO_EDGE		(1 << 2)
#define BIGPEMU_TEXFLAG_GENMIPS				(1 << 3)

#define BIGPEMU_OPRFLAG_OUTPUTRGBA			(1 << 0)
#define BIGPEMU_OPRFLAG_CLEARBUFFER			(1 << 1)

typedef enum
{
	kSharedNetDev_None = 0,
	kSharedNetDev_JagLink,
	kSharedNetDev_StateSync,
	kSharedNetDev_Script,
	kSharedNetDev_Count
} ESharedNetDeviceType;

typedef enum
{
	kSharedNetConn_None = 0,
	kSharedNetConn_Host,
	kSharedNetConn_Client
} ESharedNetConnectionType;

#define BIGPEMU_CLIENT_DEST_ALL				-1

//events should generally return 0 unless a special return value is called for based on the event type
typedef uint32_t (*TBigPEmuEventCallback)(const int eventHandle, void *pEventData);

typedef void (*TBigPEmuBPCallbackM68K)(const uint32_t addr);
typedef TBigPEmuBPCallbackM68K TBigPEmuBPCallbackRISC;

#ifdef __BIGPVM__
typedef uint32_t (*TBigPEmuSysCall)(const EBigPEmuSysCall callType, void *pData, void *pParamA, void *pParamB);
static TBigPEmuSysCall skpSysCall = (TBigPEmuSysCall)BIGPEMU_VM_SYSCALL_ADDR;
#endif

#endif //_BIGP_SHARED_H
