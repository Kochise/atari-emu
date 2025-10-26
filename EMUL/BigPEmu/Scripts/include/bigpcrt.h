#pragma once
#ifndef _BIGPCRT_H
#define _BIGPCRT_H


typedef char int8_t;
typedef unsigned char uint8_t;
typedef short int16_t;
typedef unsigned short uint16_t;
typedef int int32_t;
typedef unsigned int uint32_t;
typedef long long int64_t;
typedef unsigned long long uint64_t;
typedef uint32_t uintptr_t;
typedef int32_t intptr_t;

typedef int64_t intmax_t;
typedef uint64_t uintmax_t;

typedef uintptr_t size_t;

#ifndef NULL
	#define NULL 0
#endif

#include "bigp_shared.h"

#ifndef ENABLE_SCRIPT_ASSERTS
	#define ENABLE_SCRIPT_ASSERTS 1
#endif

#if ENABLE_SCRIPT_ASSERTS
	#define assert(...) { if (!(__VA_ARGS__)) { skpSysCall(kBPESys_FailedAssert, (void *)__FILE__, (void *)__LINE__, NULL); } }
#else
	#define assert(...) {}
#endif

#ifndef wchar_t
typedef int16_t wchar_t;
#endif

#define CHAR_BIT      8
#define SCHAR_MIN   (-128)
#define SCHAR_MAX     127
#define UCHAR_MAX     0xff

#ifndef _CHAR_UNSIGNED
    #define CHAR_MIN    SCHAR_MIN
    #define CHAR_MAX    SCHAR_MAX
#else
    #define CHAR_MIN    0
    #define CHAR_MAX    UCHAR_MAX
#endif

#define MB_LEN_MAX    5
#define SHRT_MIN    (-32768)
#define SHRT_MAX      32767
#define USHRT_MAX     0xffff
#define INT_MIN     (-2147483647 - 1)
#define INT_MAX       2147483647
#define UINT_MAX      0xffffffff
#define LONG_MIN    (-2147483647L - 1)
#define LONG_MAX      2147483647L
#define ULONG_MAX     0xffffffffUL
#define LLONG_MAX     9223372036854775807i64
#define LLONG_MIN   (-9223372036854775807i64 - 1)
#define ULLONG_MAX    0xffffffffffffffffui64

#define _I8_MIN     (-127i8 - 1)
#define _I8_MAX       127i8
#define _UI8_MAX      0xffui8

#define _I16_MIN    (-32767i16 - 1)
#define _I16_MAX      32767i16
#define _UI16_MAX     0xffffui16

#define _I32_MIN    (-2147483647i32 - 1)
#define _I32_MAX      2147483647i32
#define _UI32_MAX     0xffffffffui32

#define _I64_MIN    (-9223372036854775807i64 - 1)
#define _I64_MAX      9223372036854775807i64
#define _UI64_MAX     0xffffffffffffffffui64


#define BIGP_CRT_MAX_LOCAL_BUFFER_SIZE		4096

#define VA_ALIGN(vaPtr)				((sizeof(vaPtr) + sizeof(void*) - 1) & ~(sizeof(uintptr_t) - 1))
typedef uint8_t * va_list;
#define va_start(vaPtr, varName)	{ vaPtr = (va_list)&varName + VA_ALIGN(varName); }
#define va_arg(vaPtr, varType)		*(varType *)((vaPtr += VA_ALIGN(varType)) - VA_ALIGN(varType))
#define va_end(vaPtr)				{ vaPtr = (va_list)0; }

int bigpemu_get_api_version();
void *bigpemu_get_module_handle();

EBigPEmuPlatform bigpemu_get_platform();
uint64_t bigpemu_native_window_handle(EBigPEmuWindowPlatform *pPlatOut); //pPlatOut may be NULL
//params may be NULL, corresponds to FindWindowA under windows
uint64_t bigpemu_find_native_window(const char *pClassName, const char *pTitle);
uint64_t bigpemu_native_window_parent(const uint64_t windowHandle);
//id/params may be truncated or unnused depending on window platform
uint64_t bigpemu_send_native_window_message(const uint64_t windowHandle, const uint64_t msgId, const uint64_t wParam, const uint64_t lParam);
uint32_t bigpemu_native_window_title(char *pDst, const uint64_t windowHandle);

void bigpemu_set_platform_api(const char *pName, void *pPtr);
void *bigpemu_get_platform_api(const char *pName);
vmuptr_t bigpemu_machine_global_call(const vmuptr_t arg0, const vmuptr_t arg1);
vmuptr_t bigpemu_machine_object_call(const vmuptr_t arg0, const vmuptr_t arg1);

double bigpemu_time_ms();

float bigpemu_gamma_to_linear(const float c);
float bigpemu_linear_to_gamma(const float c);

//event register functions return an event handle, which can be fed to bigpemu_unregister_event.
//take care not to register events inside event callbacks, this could lead to thread deadlocks.
int bigpemu_register_event_sw_loaded(void *pModuleHandle, TBigPEmuEventCallback pCb);
int bigpemu_register_event_sw_unloaded(void *pModuleHandle, TBigPEmuEventCallback pCb);
int bigpemu_register_event_emu_thread_frame(void *pModuleHandle, TBigPEmuEventCallback pCb);
int bigpemu_register_event_video_thread_frame(void *pModuleHandle, TBigPEmuEventCallback pCb);
int bigpemu_register_event_pre_ui(void *pModuleHandle, TBigPEmuEventCallback pCb);
int bigpemu_register_event_post_ui(void *pModuleHandle, TBigPEmuEventCallback pCb);
int bigpemu_register_event_save_state(void *pModuleHandle, TBigPEmuEventCallback pCb, const uint64_t softHash);
int bigpemu_register_event_load_state(void *pModuleHandle, TBigPEmuEventCallback pCb, const uint64_t softHash);
int bigpemu_register_event_audio_frame(void *pModuleHandle, TBigPEmuEventCallback pCb);
int bigpemu_register_event_input_frame(void *pModuleHandle, TBigPEmuEventCallback pCb);

void bigpemu_unregister_event(void *pModuleHandle, const int eventHandle);

void bigpemu_set_module_usage_flags(void *pModuleHandle, const uint64_t usageFlags);

uint64_t bigpemu_get_loaded_fnv1a64();
void bigpemu_get_loaded_software_path(char *pDst, const uint32_t bufferSize);

//name of localized string goes in, localized string comes out. (if found) buffer must be at least BIGP_CRT_MAX_LOCAL_BUFFER_SIZE bytes. returns >=0 (string length) on success.
int bigpemu_get_localized_string(char *pInOut);

//similar to above, returns >=0 on success and the buffer pointed to by pOut must be at least BIGP_CRT_MAX_LOCAL_BUFFER_SIZE bytes.
int bigpemu_get_localized_emu_button_name(char *pOut, const uint32_t buttonIndex);

void bigpemu_set_rich_presence(const char *pMsg, const uint32_t displayTimeMs);

//returns non-0 if the menu is active
uint32_t bigpemu_menu_is_active();
//returns non-0 if we're in "portrait" mode (a resolution where height > width)
uint32_t bigpemu_is_portrait_mode();

//once settings (and their categories) are registered, they stay registered until the module is unloaded.
int bigpemu_register_setting_category(void *pModuleHandle, const char *pCatName); //returns cat handle
int bigpemu_register_setting(void *pModuleHandle, const int catHandle, const char *pSettingName, const EBigPEmuSettingType settingType, const void *pDefaultVal, const void *pMinVal, const void *pMaxVal, const void *pStepVal); //returns setting handle
int bigpemu_get_setting_value(void *pOut, const int settingHandle); //returns 0 on success
int bigpemu_set_setting_value(const void *pIn, const int settingHandle); //returns 0 on success
//convenience for common approaches (use bigpemu_register_setting directly for anything else)
static inline int bigpemu_register_setting_bool(const int catHandle, const char *pSettingName, const int32_t defaultVal)
{
	return bigpemu_register_setting(bigpemu_get_module_handle(), catHandle, pSettingName, kBPE_VMSetting_Bool, &defaultVal, NULL, NULL, NULL);
}
static inline int bigpemu_register_setting_int(const int catHandle, const char *pSettingName, const int32_t defaultVal)
{
	return bigpemu_register_setting(bigpemu_get_module_handle(), catHandle, pSettingName, kBPE_VMSetting_Int, &defaultVal, NULL, NULL, NULL);
}
static inline int bigpemu_register_setting_int_full(const int catHandle, const char *pSettingName, const int32_t defaultVal, const int32_t minVal, const int32_t maxVal, const int32_t stepVal)
{
	return bigpemu_register_setting(bigpemu_get_module_handle(), catHandle, pSettingName, kBPE_VMSetting_Int, &defaultVal, &minVal, &maxVal, &stepVal);
}
static inline int bigpemu_register_setting_float_full(const int catHandle, const char *pSettingName, const float defaultVal, const float minVal, const float maxVal, const float stepVal)
{
	return bigpemu_register_setting(bigpemu_get_module_handle(), catHandle, pSettingName, kBPE_VMSetting_Float, &defaultVal, &minVal, &maxVal, &stepVal);
}

#ifndef _BIGP_PLATFORM_NEW_

//IMPORTANT NOTE:
//!!!!!!!!!!!!!!! <--- this means read this note for real
//all bigpemu_jag_* functions should only be called from the emulator thread, such as during an emu_thread_frame event or breakpoint callback.
//there will generally not be error-handling/detection when using these functions on non-emulator threads, but that has the potential to break things in unreliable ways.
//this is a case where "works on my system" will be a meaningless sentiment if you screw up.
//END OF IMPORTANT NOTE, READ THE ABOVE NOTE, I MEAN IT, DON'T BE A FUCKER

//a lot of device state is not exposed through the api, because registers can be read/written via these individual read/write functions as on hardware.
//by default, these reads and writes all follow the 68k bus path.
uint8_t bigpemu_jag_read8(const uint32_t addr);
uint16_t bigpemu_jag_read16(const uint32_t addr);
uint32_t bigpemu_jag_read32(const uint32_t addr);
uint64_t bigpemu_jag_read64(const uint32_t addr);
void bigpemu_jag_write8(const uint32_t addr, const uint8_t val);
void bigpemu_jag_write16(const uint32_t addr, const uint16_t val);
void bigpemu_jag_write32(const uint32_t addr, const uint32_t val);
void bigpemu_jag_write64(const uint32_t addr, const uint64_t val);

//sysmem functions should generally only be used with ram/rom address ranges, results are otherwise undefined (and sometimes unreliable)
//remember that jaguar endianness is always big, and vm endianness is always little (even on big hosts for consistency's sake)
//additionally, most state data will auto-reset with the system, or sometimes when loading a saved state.
int bigpemu_jag_sysmemcmp(const void *pSrc, const uint32_t addr, const uint32_t memSize);
void bigpemu_jag_sysmemset(const uint32_t addr, const int32_t setVal, const uint32_t memSize);
void bigpemu_jag_sysmemread(void *pDst, const uint32_t addr, const uint32_t memSize);
void bigpemu_jag_sysmemwrite(const void *pSrc, const uint32_t addr, const uint32_t memSize);

//note that all breakpoints are automatically cleared each time software is loaded/reset.
//this means you'll usually want to set your breakpoints in the sw_loaded event callback.
void bigpemu_jag_m68k_bp_add(const uint32_t addr, TBigPEmuBPCallbackM68K pCb);
void bigpemu_jag_m68k_bp_del(const uint32_t addr);
void bigpemu_jag_m68k_set_pc(const uint32_t newVal);
uint32_t bigpemu_jag_m68k_get_pc();
void bigpemu_jag_m68k_set_dreg(const uint32_t regIndex, const uint32_t newVal);
uint32_t bigpemu_jag_m68k_get_dreg(const uint32_t regIndex);
void bigpemu_jag_m68k_set_areg(const uint32_t regIndex, const uint32_t newVal);
uint32_t bigpemu_jag_m68k_get_areg(const uint32_t regIndex);
void bigpemu_jag_m68k_consume_cycles(const int32_t cycleCount);

//set/get pc for the gpu/dsp are provided as a convenience. other registers should be accessed through the memory functions.
void bigpemu_jag_gpu_set_pc(const uint32_t newVal);
uint32_t bigpemu_jag_gpu_get_pc();
//set_reg/get_reg are use absolute register indices, so regIndex can be 0..63 to select from either register bank.
void bigpemu_jag_gpu_set_reg(const uint32_t regIndex, const uint32_t newVal);
uint32_t bigpemu_jag_gpu_get_reg(const uint32_t regIndex);
//the curbank/altbank functions select the register bank via REGPAGE, so regIndex should be 0..31
void bigpemu_jag_gpu_curbank_set_reg(const uint32_t regIndex, const uint32_t newVal);
uint32_t bigpemu_jag_gpu_curbank_get_reg(const uint32_t regIndex);
void bigpemu_jag_gpu_altbank_set_reg(const uint32_t regIndex, const uint32_t newVal);
uint32_t bigpemu_jag_gpu_altbank_get_reg(const uint32_t regIndex);
void bigpemu_jag_gpu_consume_cycles(const int32_t cycleCount);
void bigpemu_jag_gpu_set_pipeline_enabled(const int32_t isEnabled);

void bigpemu_jag_dsp_set_pc(const uint32_t newVal);
uint32_t bigpemu_jag_dsp_get_pc();
void bigpemu_jag_dsp_set_reg(const uint32_t regIndex, const uint32_t newVal);
uint32_t bigpemu_jag_dsp_get_reg(const uint32_t regIndex);
void bigpemu_jag_dsp_curbank_set_reg(const uint32_t regIndex, const uint32_t newVal);
uint32_t bigpemu_jag_dsp_curbank_get_reg(const uint32_t regIndex);
void bigpemu_jag_dsp_altbank_set_reg(const uint32_t regIndex, const uint32_t newVal);
uint32_t bigpemu_jag_dsp_altbank_get_reg(const uint32_t regIndex);
void bigpemu_jag_dsp_consume_cycles(const int32_t cycleCount);
void bigpemu_jag_dsp_set_pipeline_enabled(const int32_t isEnabled);

//injecting a breakpoint into risc code will stomp 8 bytes from the address. this is how we manage "free" breakpoints
//by making them part of the instruction encoding and not adding any overhead/virtualization to pc reads.
//the downside of this is that these things are invasive, you'll want to make sure you take care to replace those 8
//bytes of logic that are being stomped by this hook, unless you're just skipping over a whole swath of code.
void bigpemu_jag_inject_risc_bp(const uint32_t addr, TBigPEmuBPCallbackRISC pCb);

ESharedEmulatorDeviceType bigpemu_jag_get_device_type(const uint32_t inputIndex);
uint32_t bigpemu_jag_get_buttons(const uint32_t inputIndex);
uint32_t bigpemu_jag_get_exbuttons(const uint32_t inputIndex);
uint32_t bigpemu_jag_get_analogs(float *pAnalogsOut, const uint32_t inputIndex); //pAnalogsOut must be large enough for 8 floats

void bigpemu_jag_set_tracker_constraints_x(const int32_t bias, const int32_t div);
void bigpemu_jag_set_tracker_constraints_y(const int32_t bias, const int32_t div);

void bigpemu_jag_set_sysflags(const uint32_t flags);
uint32_t bigpemu_jag_get_sysflags();
void bigpemu_jag_set_m68k_clock_scale(double clockScale);
void bigpemu_jag_set_risc_clock_scale(double clockScale);
void bigpemu_jag_set_lockcycles(const uint32_t lockCycles);

void bigpemu_jag_jerry_set_inten(const uint32_t enableBits);
uint32_t bigpemu_jag_jerry_get_inten();
void bigpemu_jag_jerry_set_intpend(const uint32_t pendingBits);
uint32_t bigpemu_jag_jerry_get_intpend();

void bigpemu_jag_tom_set_inten(const uint32_t enableBits);
uint32_t bigpemu_jag_tom_get_inten();
void bigpemu_jag_tom_set_intpend(const uint32_t pendingBits);
uint32_t bigpemu_jag_tom_get_intpend();

void bigpemu_jag_blitter_raw_set(const EBigPEmuBlitterRaw blitterRaw, const uint32_t newVal);
uint32_t bigpemu_jag_blitter_raw_get(const EBigPEmuBlitterRaw blitterRaw);
void bigpemu_jag_blitter_set_excycles(const uint32_t exCycles);

void bigpemu_jag_set_stereo_enabled(const uint32_t flags); //(flags & 1) = enabled, (flags & 2) = duplicate eye mode
void bigpemu_jag_set_stereo_scan_eye(const uint32_t isLeft);

uint32_t bigpemu_jag_get_vmode_divisor();

void bigpemu_jag_op_set_special_transparency(const uint32_t objectSrcAddr, const uint32_t color, const EOPVMColorType colorType);
void bigpemu_jag_op_add_poly(const uint32_t objectSrcAddr, const SOPVMPolyInfo *pPolyInfo);
void bigpemu_jag_op_clear_buffers(const uint32_t objectSrcAddr);
void bigpemu_jag_op_set_alpha_fill(const int32_t isEnabled);
void bigpemu_jag_op_enable_play_area_scissor(const uint32_t isEnabled);
int32_t bigpemu_jag_op_create_frame_tex(const uint32_t objectSrcAddr, const uint32_t imgAddr, const uint32_t isJagAddr, const uint32_t width, const uint32_t height, const uint32_t pitch, const EOPVMTexType texType, const uint32_t flags, const uint32_t *pDCompEnVal);
int32_t bigpemu_jag_op_create_tex_from_res(const uint32_t objectSrcAddr, const TSharedResPtr resPtr, const uint32_t flags);
void bigpemu_jag_op_render_bitmap_object_to_buffer(uint8_t *pBuffer, uint32_t *pFlagsOut, const uint32_t width, const uint32_t height, const uint32_t objAddr, const uint32_t flags);

uint32_t bigpemu_jag_is_ntsc(); //non-0 if ntsc
uint64_t bigpemu_jag_get_frame_count();
uint32_t bigpemu_jag_get_line_count();
double bigpemu_jag_get_exec_time();
double bigpemu_jag_get_horizontal_period();
double bigpemu_jag_get_frame_period();
double bigpemu_jag_master_clock_mhz();
uint32_t bigpemu_jag_risc_cycle_for_usec(const double usec);
uint32_t bigpemu_jag_m68k_cycle_for_usec(const double usec);
double bigpemu_jag_usec_for_risc_cycle(const uint32_t cyc);
double bigpemu_jag_usec_for_m68k_cycle(const uint32_t cyc);

//rectangle is in the form of x0 (left), x1 (right), y0 (top), y1 (bottom)
//x units are pixel clocks, y units are progressive lines
void bigpemu_jag_get_display_region(int32_t *pRect);

//pRect 0,1,2,3=ul x, lr x, ul y, lr y. x should be specified in pixel clocks. pass a null pointer to reset.
void bigpemu_jag_force_display_bounds(const int32_t *pRect);
//can be used to force a given aspect ratio, e.g. 320.0 / 240.0. set to 0 to reset.
void bigpemu_jag_force_display_ratio(const double aspectRatio);

uint32_t bigpemu_jag_get_rw_handler_alignment();
//returns 1 on success. addresses must be aligned to whatever bigpemu_jag_get_rw_handler_alignment returns.
//addresses must also be in a valid/supported region. ram/rom addresses are not supported for performance reasons.
//rwMask should be some combination of BIGPEMU_HANDLER_MASK_* values. non-inclusive of endAddr.
//all handlers will be reset on software load, so it's a good idea to install them in a "software loaded" handler.
uint32_t bigpemu_jag_set_rw_handler(const TBigPEmuReadWriteCallback pCb, const uint32_t startAddr, const uint32_t endAddr, const uint32_t rwMask);

//the following jag functions are thread-safe
void bigpemu_jag_set_paused(const uint32_t isPaused);

#else //_BIGP_PLATFORM_NEW_

//the same threading rules apply to bigpemu_emu_* functions as those outlined for bigpemu_jag_* functions above.
	
uint64_t bigpemu_emu_get_frame_count();

//the following emu functions are thread-safe
void bigpemu_emu_set_paused(const uint32_t isPaused);

#endif //_BIGP_PLATFORM_NEW_

//input functions are thread-safe and may be used from any thread.
//the input data structure is opaque and may vary per input plugin, so bigpemu_input_get_input_size must be used to determine how much space is needed for input buffers.
uint32_t bigpemu_input_get_input_size();
uint32_t bigpemu_input_get_input_data_version();
uint32_t bigpemu_input_get_all_held_inputs(void *pInputBuffer, const uint32_t maxHeldCount);
uint32_t bigpemu_input_create_input_from_vk(void *pInputBuffer, const uint32_t vk); //input buffer must be at least bigpemu_input_get_input_size() byte, vk uses windows vk codes (even on non-windows platforms), returns non-0 on success
//raw data comparison may not yield correct results when comparing inputs to, for example, see if a button is held.
//this function should be used instead of raw comparisons to see if a given input is in a set, e.g. the set returned by bigpemu_input_get_all_held_inputs.
//the return value will be non-0 if the single input is in the set.
uint32_t bigpemu_input_input_in_set(const void *pSetBuffer, const uint32_t setEntryCount, const void *pSingleInputBuffer);
uint32_t bigpemu_input_get_input_name(char *pDst, const uint32_t dstSize, const void *pInputBuffer); //returns length of string on success, 0 on failure. fails of dstSize is too small.
uint32_t bigpemu_input_get_device_count();

//all res functions may only be called from the main thread.
//correctly freeing resources is encouraged, but all resources are associated with the module and will be auto-freed when the module is freed.
TSharedResPtr bigpemu_res_texture_from_png(void *pModuleHandle, uint32_t *pWidthOut, uint32_t *pHeightOut, const uint8_t *pData, const uint32_t dataSize, const uint32_t texFlags);
TSharedResPtr bigpemu_res_texture_from_rgba32(void *pModuleHandle, const uint8_t *pData, const uint32_t width, const uint32_t height, const uint32_t texFlags);
void bigpemu_res_texture_rgba32_reupload(TSharedResPtr pRes, void *pModuleHandle, const uint8_t *pData, const uint32_t width, const uint32_t height, const uint32_t texFlags);
TSharedResPtr bigpemu_res_texture_rgba32_create_or_reupload(TSharedResPtr pRes, void *pModuleHandle, const uint8_t *pData, const uint32_t width, const uint32_t height, const uint32_t texFlags);
void bigpemu_res_texture_free(void *pModuleHandle, TSharedResPtr pRes);
TSharedResPtr bigpemu_res_sound_from_wave(void *pModuleHandle, const uint8_t *pData, const uint32_t dataSize);
TSharedResPtr bigpemu_res_sound_from_mod(void *pModuleHandle, const uint8_t *pData, const uint32_t dataSize);
TSharedResPtr bigpemu_res_sound_from_mp3(void *pModuleHandle, const uint8_t *pData, const uint32_t dataSize);
void bigpemu_res_sound_free(void *pModuleHandle, TSharedResPtr pRes);

//all drawui functions may only be called from pre-ui and post-ui callbacks
void bigpemu_drawui_get_virtual_canvas_size(float *pWHOut);
void bigpemu_drawui_get_virtual_to_native_scales(float *pXYOut);
//the display rectangle represents the display area for the emulated system.
//rectangle is in the form of x0 (left), x1 (right), y0 (top), y1 (bottom)
void bigpemu_drawui_get_virtual_display_rect(float *pRect);
void bigpemu_drawui_text_ex(const char *pText, const float x, const float y, const float scale, const float *pRgba, const EDrawUITextJustify tj, const float wrapDist);
void bigpemu_drawui_text(const char *pText, const float x, const float y, const float scale);
uint32_t bigpemu_drawui_text_bounds_ex(float *pRectOut, const char *pText, const float scale, const float wrapDist); //returns 0 on failure
uint32_t bigpemu_drawui_text_bounds(float *pRectOut, const char *pText, const float scale); //returns 0 on failure
//pTexture can be 0 for untextured rectangles, pSecRgba can be 0 for single-color rectangles
void bigpemu_drawui_rect(TSharedResPtr pTexture, const float x, const float y, const float w, const float h, const float *pRgba, const float *pSecRgba);
void bigpemu_drawui_outlined_rect(const float x, const float y, const float w, const float h, const float *pRgba, const float *pSecRgba, const float borderWidth, const float *pBorderRgba);
//each point must be vec3, normally the z component can just be 0.
void bigpemu_drawui_lines_ex(const float *pPoints, const uint32_t pointCount, const float width, const float *pRgba, const float innerWidth, const float hardness, const float *pSecRgba, const float attnDist);
void bigpemu_drawui_lines(const float *pPoints, const uint32_t pointCount, const float width, const float *pRgba);

uint32_t bigpemu_touch_count();
//note that the actual touch at a given index may change across frames.
//you should always use id instead of index to track touches across frames.
//additionally, touches should always be queried on the main thread. there is the potential to miss single-frame touches
//when querying from the emulator thread.
void bigpemu_touch_info(TBigPEmuTouchInfo *pInfo, const int32_t touchIndex);
//returns -1 if the touch id given is no longer active
int32_t bigpemu_touch_index_for_id(const uint64_t id);
//returns non-0 (BIGPEMU_TOUCHOL_* bits) if any touch overlay elements are intersected at the given position
uint64_t bigpemu_touch_intersecting_overlay(const float *pPos, const float *pSize);
void bigpemu_touch_set_prefer_hidden_elems(const uint64_t hideBits);

//all listener params are 3d vectors (3 floats each)
int32_t bigpemu_audio_loadir(const uint8_t *pData, const uint32_t dataSize); //pass NULL, 0 to get an index to the default/embedded hrir
void bigpemu_audio_update_listener(const float *pPos, const float *pFwd, const float *pRight, const float *pUp);
void bigpemu_audio_default_spatial(TBigPEmuSpatialAudio *pSpatial);
void bigpemu_audio_play_sound(TSharedResPtr pRes);
//ex version returns a sound id
uint64_t bigpemu_audio_play_sound_ex(TSharedResPtr pRes, const uint32_t soundFlags, const TBigPEmuSpatialAudio *pSpatial);
void bigpemu_audio_sound_update_spatial(const uint64_t soundId, const TBigPEmuSpatialAudio *pSpatial);
void bigpemu_audio_sound_stop(const uint64_t soundId);
uint32_t bigpemu_audio_sound_finished(const uint64_t soundId); //returns non-0 if sound is no longer playing

void bigpemu_audio_resample(int16_t *pDst, const uint32_t dstCount, const int16_t *pSrc, const uint32_t srcCount, const uint32_t channelCount);
//optionally, pDstRight may be NULL to automatically mix channels into pDstLeft
void bigpemu_audio_stereo_deinterleave_and_expand(float *pDstLeft, float *pDstRight, const int16_t *pSrcSamples, const uint32_t sampleCount);
void bigpemu_audio_stereo_interleave_and_compress(int16_t *pDstSamples, const float *pSrcLeft, const float *pSrcRight, const uint32_t sampleCount);
void bigpemu_audio_mix_int16(int16_t *pInOut, const int16_t *pIn, const uint32_t sampleCount, const uint32_t flags); //(flags & 1) == treat samples as big-endian
//note that pDftData requires sizeof(double) * 2 * sampleCount storage (interleaved real-imaginary values)
void bigpemu_dft(double *pDftData, const float *pSampleData, const uint32_t sampleCount);
void bigpemu_idft(float *pSampleData, const double *pDftData, const uint32_t sampleCount);
void bigpemu_scale_signal(float *pSampleData, const uint32_t sampleCount, const float f);
void bigpemu_bias_signal(float *pSampleData, const uint32_t sampleCount, const float f);
void bigpemu_clamp_signal(float *pSampleData, const uint32_t sampleCount, const float minVal, const float maxVal);
void bigpemu_replace_signal(float *pSampleData, const uint32_t sampleCount, const float minVal, const float maxVal, const float newVal);
void bigpemu_rotate_signal_buffer(float *pSampleData, const uint32_t sampleCount, const uint32_t offset);
float bigpemu_get_signal_mean(float *pSampleData, const uint32_t sampleCount);
float bigpemu_get_signal_max(float *pSampleData, const uint32_t sampleCount);
float bigpemu_get_signal_min(float *pSampleData, const uint32_t sampleCount);
void bigpemu_quantize_dft_amplitudes(float *pBuckets, const double *pDftData, const uint32_t bucketCount, const uint32_t sampleCount, const uint32_t flags);
void bigpemu_render_signal_rgba32(uint8_t *pRgbaOut, const uint32_t widthOut, const uint32_t heightOut, const float *pSamples, const uint32_t sampleCount, const float *pColorLow, const float *pColorHigh);

int bigpemu_register_event_net_update(void *pModuleHandle, TBigPEmuEventCallback pCb);
int bigpemu_register_event_net_receive(void *pModuleHandle, TBigPEmuEventCallback pCb);
ESharedNetDeviceType bigpemu_net_current_device();
ESharedNetConnectionType bigpemu_net_connection_type();
int32_t bigpemu_net_client_index(); //may only be called inside of net callbacks
uint64_t bigpemu_net_client_sw_hash(const int32_t clientIndex); //may only be called inside of net callbacks
//may only be called inside of a net update callback. returns dataSize with all data was sent successfully. dataSize is currently restricted to 64KB, and must be aligned to 4 bytes.
//clientDestIndex should be BIGPEMU_CLIENT_DEST_ALL to send to all active connections.
//it's important to avoid changing the data size for a given id, this will force a full flush and prevent any delta compression from occurring.
uint32_t bigpemu_net_send(const uint32_t id, const int32_t clientDestIndex, const void *pData, const uint32_t dataSize);
//bigpemu_net_send_elems operates like bigpemu_net_send, but auto-increments the message id for each element
uint32_t bigpemu_net_send_elems(const uint32_t startId, const int32_t clientDestIndex, const void *pData, const uint32_t elemSize, const uint32_t elemCount);
//raw, uncompressed message
uint32_t bigpemu_net_send_nodelta(const uint32_t id, const int32_t clientDestIndex, const void *pData, const uint32_t dataSize);
//may only be called inside of a net receive callback. if the returned value is larger than the dataSize passed in, the pData buffer isn't large enough and will not be modified.
uint32_t bigpemu_net_recv(uint32_t *pIdOut, void *pData, const uint32_t dataSize);
//returns non-0 if the client (also allows BIGPEMU_CLIENT_DEST_ALL) is behind on traffic. may be used to throttle state updates under poor network performance.
uint32_t bigpemu_net_behind(const int32_t clientDestIndex);
uint32_t bigpemu_net_hostmsg(char const *pFmt, ...); //may only be used on host
uint32_t bigpemu_net_disconnect(const int32_t clientDestIndex);
int32_t bigpemu_net_lastclient();
//only valid from host, returns non-0 (string length) on success.
//likely to return 0 in various circumstances, such as client information not yet arriving on host, disconnection, insufficient buffer size
uint32_t bigpemu_net_client_name(char *pBuffer, const uint32_t bufferSize, const uint32_t clientIndex);

//looks in the last used rom image directory for supported file types. returns the number of names in the pNameOffsets array, if any. may not be called from the emulator thread.
uint32_t bigpemu_get_rom_dir_list(char *pNamesBuffer, const uint32_t namesBufferSize, uint32_t *pNameOffsets, const uint32_t maxNameCount);
//loads an image from the last used rom image directory. accepts local names only. returns non-0 on success. may not be called from the emulator thread.
uint32_t bigpemu_load_rom_from_current_dir(const char *pName);

void bigpemu_set_named_var_data(const char *pName, const void *pData, const uint32_t dataSize);
void bigpemu_get_named_var_data(void *pData, const char *pName, const uint32_t dataSize);

//returns < 0 on failure (this means all overlay elements are claimed), otherwise the ol elem index
int32_t bigpemu_claim_user_ol_elem();
void bigpemu_set_user_ol_elem_data(const int32_t userElemIndex, const char *pLabel, const uint8_t *pInputData, const uint32_t inputCount, void *pResv);

//returns non-0 on success
uint32_t bigpemu_get_cfg_data_blob(void *pBlobData, uint32_t *pBlobSize, const uint32_t dataMaxSize, const char *pBlobName);
uint32_t bigpemu_set_cfg_data_blob(const void *pBlobData, const uint32_t blobSize, const char *pBlobName);

//these functions can be ok for one-off uses, but it's generally not recommend that you use them repeatedly.
//performance is not good (not optimized at all), and unfreed allocs will remain leaked even after your module is unloaded.
//in most cases it's a better idea to create your own large static buffer, and if heap allocation is really needed, allocate your own heap from your static buffer and claim
//new (large) pages via the vm alloc only if really necessary.
void *bigpemu_vm_alloc(const uint32_t allocSize);
void bigpemu_vm_free(void *pPtr);

//similar to filesystem calls, native modules must exist under the "Scripts" directory, and names/paths provided are relative to that location.
//THE NATIVE MODULE PLATFORM MUST MATCH THE HOST PLATFORM. that is to say, for Win64 builds of BigPEmu, only Win64 DLL files can be employed through this interface.
uint64_t bigpemu_nativemod_load(const char *pName);
void bigpemu_nativemod_free(const uint64_t modHandle);
//another important note - only the CDECL calling convention is supported here.
//if you need non-CDECL, you should wrap your native calls with another DLL which exposes only CDECL functions, which can then act as native relay functions.
//this is because i'm using a fast and lazy trick of not giving a shit about function prototypes or atomizing what needs to go on the stack, i just figured out
//how much data the function call is putting on the stack and pass it through to the native function.
uint64_t bigpemu_nativemod_getfuncaddr(const uint64_t modHandle, const char *pFuncName);
//this is a pretty sneaky scheme, we preppare the vm for a native call and then make our native call into the actual syscall address, placing the data we'll need on the vm stack to be translated for the native call.
#define bigpemu_nativemod_call(pFuncPtr, modHandle, nativeFuncAddr, ...) \
{ \
	TNativeCallInfo callInfo; \
	callInfo.mMod = modHandle; \
	callInfo.mFuncAddr = nativeFuncAddr; \
	*(uintptr_t *)&pFuncPtr = (uintptr_t)BIGPEMU_VM_SYSCALL_ADDR; \
	skpSysCall(kBPE_PrepareForNativeCall, &callInfo, NULL, NULL); \
	pFuncPtr(__VA_ARGS__); \
}
#define bigpemu_nativemod_call_ret(pRetVal, pFuncPtr, modHandle, nativeFuncAddr, ...) \
{ \
	TNativeCallInfo callInfo; \
	callInfo.mMod = modHandle; \
	callInfo.mFuncAddr = nativeFuncAddr; \
	*(uintptr_t *)&pFuncPtr = (uintptr_t)BIGPEMU_VM_SYSCALL_ADDR; \
	skpSysCall(kBPE_PrepareForNativeCall, &callInfo, NULL, NULL); \
	*pRetVal = pFuncPtr(__VA_ARGS__); \
}

uint64_t bigpemu_process_memory_alias(void *pPtr);

//some random vector/matrix functions which may or may not be native-backed. (faster than anything the interpreter can give you)
//likely to grow as i do more mathy shit in scripts.
void math_vec3_copy(float *pDst, const float *pSrc);
void math_vec3_add(float *pDst, const float *pSrc);
void math_vec3_add2(float *pDst, const float *pSrc1, const float *pSrc2);
void math_vec3_sub(float *pDst, const float *pSrc);
void math_vec3_sub2(float *pDst, const float *pSrc1, const float *pSrc2);
void math_vec3_scale(float *pDst, const float *pSrc);
void math_vec3_scale2(float *pDst, const float *pSrc1, const float *pSrc2);
void math_vec3_scalef(float *pDst, const float f);
void math_vec3_add_scaled(float *pDst, const float *pSrcAdd, const float *pSrcMul);
float math_vec3_dot(const float *pSrc1, const float *pSrc2);
float math_vec3_lengthsq(const float *pSrc);
float math_vec3_length(const float *pSrc);
float math_vec3_normalize(float *pDst);
void math_vec3_cross(float *pDst, const float *pSrc1, const float *pSrc2);
void math_vec3_clampf(float *pDst, const float fMin, const float fMax);
void math_mat44_identity(float *pDst);
void math_mat44_multiply(float *pDst, const float *pSrc1, const float *pSrc2);
void math_mat44_inverse(float *pDst, const float *pSrc);
void math_mat44_rotate(float *pDst, const float angle, const float *pXyz); //opengl convention
void math_mat44_translate(float *pDst, const float *pVec);
void math_mat44_transform_vec3(float *pOut, const float *pMat, const float *pVec);
void math_mat44_transform_vec4(float *pOut, const float *pMat, const float *pVec);

uint32_t math_clip_axially_aligned_quad_to_canvas(int32_t *pClipPos, int32_t *pClipSize, const int32_t x, const int32_t y, const int32_t w, const int32_t h, const int32_t canvW, const int32_t canvH);
//each point must start with a floating point x/y, points will be clipped to a 0..1 (not -1..1) region
uint32_t math_clip_2d_polygon_to_ndc(void *pPoints, const uint32_t pointStride, const uint32_t countIn);
float math_float_approach_value(const float from, const float to, const float speed);

//half-assed/partially-implemented standard crt functions. printf-style formatting is a little hit or miss in terms of available identifiers.
//sorry if your favorite functions are missing. this is a land of vm madness, so you can always implement them yourself!

double atof(const char *pStr);
int atoi(const char *pStr);
unsigned long strtoul(const char *pStr, char **ppEndPtr, int radix);
long strtol(const char *pStr, char **ppEndPtr, int radix);
double strtod(const char *pStr, char **ppEndPtr);

float sinf(const float x);
float cosf(const float x);
float tanf(const float x);
float asinf(const float x);
float acosf(const float x);
float atanf(const float x);
float atan2f(const float y, const float x);
float expf(const float x);
float fmodf(const float x, const float y);
float ceilf(const float x);
float floorf(const float x);
float logf(const float x);
float powf(const float x, const float y);
float fabsf(const float x);
float sqrtf(const float x);

double sin(const double x);
double cos(const double x);
double tan(const double x);
double asin(const double x);
double acos(const double x);
double atan(const double x);
double atan2(const double y, const double x);
double exp(const double x);
double fmod(const double x, const double y);
double ceil(const double x);
double floor(const double x);
double log(const double x);
double pow(const double x, const double y);
double fabs(const double x);
double sqrt(const double x);

//printf will only go to the log (if enabled), while printf_notify will also go through the menu/HUD notify system and be auto-endlined.
int printf(char const *pFmt, ...);
int printf_notify(char const *pFmt, ...);
int sprintf(char *pBuf, const char *pFmt, ...);
int vsprintf(char *pBuf, const char *pFmt, va_list ap);
int sscanf(const char *pBuf, const char *pFmt, ...);

char *strcpy(char *pDst, const char *pSrc);
char *strncpy(char *pDst, const char *pSrc, const size_t copySize);
size_t strlen(const char *pStr);
char *strcat(char *pDst, const char *pSrc);
int strcmp(const char *pS1, const char *pS2);
int stricmp(const char *pS1, const char *pS2);
int strnicmp(const char *pS1, const char *pS2, const size_t cmpSize);
char *strstr(const char *pS1, const char *pS2);
int toupper(const int c);
int tolower(const int c);
int isspace(const int c);
int isdigit(const int c);

void *memcpy(void *pDst, const void *pSrc, const size_t copySize);
void *memset(void *pDst, const int setVal, const size_t setSize);
int memcmp(const void *pS1, const void *pS2, const size_t cmpSize);

void srand(const uint32_t seed);
int rand();

uint16_t byteswap_ushort(const uint16_t v);
uint32_t byteswap_ulong(const uint32_t v);

//vm fs capabilities are intentionally limited. "Scripts" is the filesystem root, files outside of the root can't be accessed.
//only basic binary read-write functionality is permitted.
uint64_t fs_open(const char *pPath, const int forWrite); //return value of 0 means failed
uint64_t fs_read(void *pBuffer, const uint64_t readSize, const uint64_t fh);
uint64_t fs_write(const void *pBuffer, const uint64_t writeSize, const uint64_t fh);
uint64_t fs_get_size(const uint64_t fh);
void fs_close(const uint64_t fh);

#define BIGPEMU_VK_BACK			0x08
#define BIGPEMU_VK_TAB			0x09
#define BIGPEMU_VK_RETURN		0x0D
#define BIGPEMU_VK_CONTROL		0x11
#define BIGPEMU_VK_ESCAPE		0x1B
#define BIGPEMU_VK_SPACE		0x20
#define BIGPEMU_VK_LEFT			0x25
#define BIGPEMU_VK_UP			0x26
#define BIGPEMU_VK_RIGHT		0x27
#define BIGPEMU_VK_DOWN			0x28
#define BIGPEMU_VK_NUMPADKEYS	0x60 //0 .. 9
#define BIGPEMU_VK_FKEYS		0x70 //F1 .. F24
//A.. Z and 0..9 correspond to ASCII ranges

#define BIGPEMU_MAX(a, b) (((a) >= (b)) ? a : b)
#define BIGPEMU_MIN(a, b) (((a) <= (b)) ? a : b)
#define BIGPEMU_SWAP16(v) (((v >> 8) & 0xFF) | ((v & 0xFF) << 8))
#define BIGPEMU_SWAP32(v) (BIGPEMU_SWAP16(v >> 16) | ((uint32_t)BIGPEMU_SWAP16(v) << 16))
#define BIGPEMU_ARRAY_LENGTH(a) (sizeof(a) / sizeof(a[0]))

#ifdef __BIGPVM_NATIVEBE__
	#define BIGPEMU_NATIVE_SWAP16(v) (v)
	#define BIGPEMU_NATIVE_SWAP32(v) (v)
#else
	#define BIGPEMU_NATIVE_SWAP16(v) BIGPEMU_SWAP16(v)
	#define BIGPEMU_NATIVE_SWAP32(v) BIGPEMU_SWAP32(v)
#endif

#endif //_BIGPCRT_H
