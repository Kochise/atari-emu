//__BIGPEMU_SCRIPT_MODULE__
//__BIGPEMU_META_DESC__		"A Hello World BigPEmu scripting sample."
//__BIGPEMU_META_AUTHOR__	"Rich Whitehouse"
//the line "//__BIGPEMU_SCRIPT_MODULE__" must be the very first line of the script, or it won't be built as a unique module.
//all subsequent META lines must follow directly, or they'll be ignored.
//for large-project organization, it's generally wise to have a single module which begins with //__BIGPEMU_SCRIPT_MODULE__,
//and which #includes additional modules which do not begin with //__BIGPEMU_SCRIPT_MODULE__.

#include "bigpcrt.h"

//the BigPEmu VM CRT expects the following functions to be implemented for every individually-built script module:
/*
void bigp_init();
void bigp_shutdown();
*/

static int sOnLoadEvent = -1;
static int sOnUnloadEvent = -1;
static int sOnVideoEvent = -1;
static int sOnPostUIEvent = -1;

static int sPrintEventsSettingHandle = -1;

static int sLastPrintEvents = 0;

static TSharedResPtr sPicTexRes = 0;
static uint32_t sPicTexWidth = 0;
static uint32_t sPicTexHeight = 0;
static float sPicTexWidthToHeight = 1.0f;

#define INPUT_BUFFER_SIZE 256
static uint8_t sInputBuffer[INPUT_BUFFER_SIZE];
static uint32_t sInputSize = 0;
static uint32_t sMaxInputsInBuffer = 0;

static const uint32_t skHotKeyVKs[] = { 0x11 /*VK_CONTROL*/, 'R' };
static uint8_t sHotKeyInputBuffer[INPUT_BUFFER_SIZE];

#define ROM_PATH_BUFFER_SIZE 1024

static uint32_t on_sw_loaded(const int eventHandle, void *pEventData)
{
	if (sLastPrintEvents)
	{
		const uint64_t hash = bigpemu_get_loaded_fnv1a64();
		//we could, for example, add some breakpoints here if the hash matches some particular software title we're after
		printf_notify("hello_pussycat loaded event: 0x%08X%08X", (uint32_t)(hash >> 32ULL), (uint32_t)(hash & 0xFFFFFFFFULL));
		
		char romPath[ROM_PATH_BUFFER_SIZE];
		bigpemu_get_loaded_software_path(romPath, ROM_PATH_BUFFER_SIZE);
		printf("Software path: %s\n", romPath);
	}
	return 0;
}

static uint32_t on_sw_unloaded(const int eventHandle, void *pEventData)
{
	if (sLastPrintEvents)
	{
		//you won't typically have much to do here, since breakpoints are auto-cleared as software is unloaded.
		printf_notify("hello_pussycat unloaded event.");
	}
	return 0;
}

//note that the interpreter will never be running on multiple threads concurrently, so we don't need to worry about thread safety between script functions.
static uint32_t on_video_frame(const int eventHandle, void *pEventData)
{
	//the video frame event is a good place to do any global state refresh which doesn't involve messing with emulated state.
	//anything which directly manipulates/reads jaguar state should instead be done in an emulation frame event.
	//failure to handle this correctly can cause threading-based issues which may be difficult to debug/reproduce.
	bigpemu_get_setting_value(&sLastPrintEvents, sPrintEventsSettingHandle);
	return 0;
}

static uint32_t on_post_ui(const int eventHandle, void *pEventData)
{
	//using this system, you could potentially create your own customizable bind interface, and load/store your own configuration using the fs functions.
	//for demo purposes, we've just hardcoded our desired key combo via skHotKeyVKs.
	const uint32_t heldCount = bigpemu_input_get_all_held_inputs(sInputBuffer, sMaxInputsInBuffer);
	if (!heldCount)
	{
		return 0;
	}

	//make sure all the desired keys are held
	for (uint32_t hkIndex = 0; hkIndex < BIGPEMU_ARRAY_LENGTH(skHotKeyVKs); ++hkIndex)
	{
		if (!bigpemu_input_input_in_set(sInputBuffer, heldCount, &sHotKeyInputBuffer[hkIndex * sInputSize]))
		{
			return 0;
		}
	}
	
	//draw the display text in the upper-right corner of the screen
	const char *pDisplayText = "Hello Pussycat!";
	const double curTime = bigpemu_time_ms();
	const float textScale = 1.25f + (float)sin(curTime * 0.001) * 0.25f;

	//ui is in virtual 640x480 coordinates when using a 4:3 native display, but virtual coordinates will adjust according to the native aspect ratio
	float uiWH[2];
	bigpemu_drawui_get_virtual_canvas_size(uiWH);
	
	float textRect[4]; //xMin, yMin, xMax, yMax
	if (bigpemu_drawui_text_bounds(textRect, pDisplayText, textScale))
	{
		const float screenEdgeDist = 8.0f;
		const float textWidth = textRect[2] - textRect[0];
		const float textHeight = textRect[3] - textRect[1];
		
		const float rectEdgeDist = 4.0f;
		const float rectWidth = textWidth + rectEdgeDist * 2.0f;
		const float rectHeight = textHeight + rectEdgeDist * 2.0f;
		const float rectX = uiWH[0] - rectWidth - screenEdgeDist;
		const float rectY = screenEdgeDist;
		
		const float rectColorTop[4] = { 0.0f, 0.0f, 0.0f, 0.9f };
		const float rectColorBottom[4] = { 0.25f, 0.0f, 0.0f, 0.9f };
		const float rectColorBorder[4] = { 0.0f, 1.0f, 0.0f, 0.8f };
		//draw a rectangle which directly fits the text at whatever scale we've provided
		bigpemu_drawui_outlined_rect(rectX, rectY, rectWidth, rectHeight, rectColorTop, rectColorBottom, 1.0f, rectColorBorder);
		
		//subtracting the mins clamps the text to the rectangle edge - strings may otherwise start inset according to the natural font displacement
		bigpemu_drawui_text(pDisplayText, rectX + rectEdgeDist - textRect[0], rectY + rectEdgeDist - textRect[1], textScale);
		
		//draw the dick face, assuming our png load was successful
		if (sPicTexRes)
		{
			const float picWidth = 92.0f * textScale;
			const float picHeight = picWidth * sPicTexWidthToHeight; //don't base the draw size on the actual image size, but do keep aspect ratio correct
			
			bigpemu_drawui_rect(sPicTexRes, rectX, rectY + rectHeight + 4.0f, picWidth, picHeight, NULL, NULL);
		}
	}
	
	return 0;
}

void bigp_init()
{
	void *pMod = bigpemu_get_module_handle();
	
	const int catHandle = bigpemu_register_setting_category(pMod, "Hello Pussycat");
	sPrintEventsSettingHandle = bigpemu_register_setting_bool(catHandle, "Print Events", 1);
	
	sOnLoadEvent = bigpemu_register_event_sw_loaded(pMod, on_sw_loaded);
	sOnUnloadEvent = bigpemu_register_event_sw_unloaded(pMod, on_sw_unloaded);
	sOnVideoEvent = bigpemu_register_event_video_thread_frame(pMod, on_video_frame);
	sOnPostUIEvent = bigpemu_register_event_post_ui(pMod, on_post_ui);
	
	sInputSize = bigpemu_input_get_input_size();
	sMaxInputsInBuffer = BIGPEMU_MIN(INPUT_BUFFER_SIZE / sInputSize, 8);
	assert(sMaxInputsInBuffer >= BIGPEMU_ARRAY_LENGTH(skHotKeyVKs));
	
	//construct the buffer for our test key combo
	for (uint32_t hkIndex = 0; hkIndex < BIGPEMU_ARRAY_LENGTH(skHotKeyVKs); ++hkIndex)
	{
		bigpemu_input_create_input_from_vk(&sHotKeyInputBuffer[hkIndex * sInputSize], skHotKeyVKs[hkIndex]);
	}
	
	printf("hello_pussycat init\nScript API version: %i\nModule handle: %08X\n", bigpemu_get_api_version(), (uint32_t)pMod);
	
	const uint64_t fh = fs_open("hello_pussycat.png", 0);
	if (fh)
	{
		const uint32_t pngSize = (uint32_t)fs_get_size(fh);
		uint8_t *pPngData = (uint8_t *)bigpemu_vm_alloc(pngSize);
		fs_read(pPngData, pngSize, fh);
		fs_close(fh);

		sPicTexRes = bigpemu_res_texture_from_png(pMod, &sPicTexWidth, &sPicTexHeight, pPngData, pngSize, BIGPEMU_TEXFLAG_BILINEAR | BIGPEMU_TEXFLAG_CLAMP_TO_EDGE | BIGPEMU_TEXFLAG_GENMIPS);
		if (sPicTexRes)
		{
			sPicTexWidthToHeight = (sPicTexWidth) ? (float)sPicTexHeight / (float)sPicTexWidth : 1.0f;
		}
		//data can be freed once the resource is created
		bigpemu_vm_free(pPngData);
	}
	
	//this demonstrates how to call the following native module function:
	/*
		extern "C" __declspec(dllexport) int hello_pussy_test_buffer(char *pOutMsg)
		{
			strcpy(pOutMsg, "hello pussy from native dll!\n");
			return 666;
		}
	*/
	//as compiled into a 64-bit Windows DLL.
	//IMPORTANT - this method only supports functions with the CDECL calling convention! we're not tracking anything about the function being called,
	//all we do is check the stack delta to see how much data the function being called expects on the stack. we then transfer it to the native stack
	//to make the call. naturally, we can only remain blind about function protos with CDECL. if you need to call non-CDECL methods, you should use a
	//wrapper module.
	/*
	const uint64_t modHandle = bigpemu_nativemod_load("hello_pussy_dll");
	if (modHandle)
	{
		//since we're expecting a 64-bit module here, we don't define the function with a char ptr, and instead explicitly use a 64-bit type.
		//we could potentially check bigpemu_get_platform here to determine which protoype to employ.
		typedef int (*THelloPussyTestBuffer)(uint64_t pOutMsg);
		const uint64_t funcAddr = bigpemu_nativemod_getfuncaddr(modHandle, "hello_pussy_test_buffer");
		if (funcAddr)
		{
			char buffer[256];
			const uint64_t bufPtr = bigpemu_process_memory_alias(buffer);
			int retVal;
			THelloPussyTestBuffer pLocalFuncPtr;
			buffer[0] = 0;

			//take care to not have function arguments evaluated in the macro call, might be a tcc bug
			bigpemu_nativemod_call_ret(&retVal, pLocalFuncPtr, modHandle, funcAddr, bufPtr);
			printf("Native module function returns %i and says: %s\n", retVal, buffer);
		}
		bigpemu_nativemod_free(modHandle);
	}
	*/
}

void bigp_shutdown()
{
	void *pMod = bigpemu_get_module_handle();
	//not strictly necessary since these events will be destroyed with the module, but we're establishing good practices here.
	bigpemu_unregister_event(pMod, sOnLoadEvent);
	bigpemu_unregister_event(pMod, sOnUnloadEvent);
	bigpemu_unregister_event(pMod, sOnVideoEvent);
	bigpemu_unregister_event(pMod, sOnPostUIEvent);
	sOnLoadEvent = -1;
	sOnUnloadEvent = -1;
	sOnVideoEvent = -1;
	sOnPostUIEvent = -1;
	sPrintEventsSettingHandle = -1;
	sLastPrintEvents = 0;
	if (sPicTexRes)
	{
		bigpemu_res_texture_free(pMod, sPicTexRes);
		sPicTexRes = 0;
	}

	printf("hello_pussycat shutdown\n");
}
