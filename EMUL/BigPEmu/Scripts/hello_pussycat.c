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

#define USE_SPECTRUM_TEXTURE() 0

#define DEMONSTRATE_RW_HANDLER() 0

#define DEMONSTRATE_SOUND_PLAYBACK() 0

static int sOnLoadEvent = -1;
static int sOnUnloadEvent = -1;
static int sOnVideoEvent = -1;
static int sOnPostUIEvent = -1;
static int sOnAudioFrameEvent = -1;
#if DEMONSTRATE_RW_HANDLER()
static int sOnEmuThreadFrameEvent = -1;
#define DEMO_START_ADDR		0xDFFF00
#define DEMO_END_ADDR		0xE00000
#endif
#if DEMONSTRATE_SOUND_PLAYBACK()
static uint64_t sSoundId = 0;
static TSharedResPtr sSoundRes = 0;
#endif

static int sPrintEventsSettingHandle = -1;

static int sLastPrintEvents = 0;

static uint64_t sLoadedHash = 0;

static TSharedResPtr sPicTexRes = 0;
static uint32_t sPicTexWidth = 0;
static uint32_t sPicTexHeight = 0;
static float sPicTexWidthToHeight = 1.0f;

#if USE_SPECTRUM_TEXTURE()
static TSharedResPtr sSpecTexRes = 0;
#endif

#define INPUT_BUFFER_SIZE 256
static uint8_t sInputBuffer[INPUT_BUFFER_SIZE];
static uint32_t sInputSize = 0;
static uint32_t sMaxInputsInBuffer = 0;

static const uint32_t skHotKeyVKs[] = { BIGPEMU_VK_CONTROL, 'R' };
static uint8_t sHotKeyInputBuffer[INPUT_BUFFER_SIZE];
static uint32_t sHotKeyDataVersion = BIGPEMU_INPUT_DATA_VERSION_INVALID;

#define FREQ_BUCKET_COUNT			32
#define AUDIO_FRAME_HISTORY_SIZE	30
#define AUDIO_MIN_VIS_FREQUENCY		20.0f
#define AUDIO_BUCKET0_SCALE			0.5f //visual preference hack, given the nature of frequency distribution after dft
static float sSampleBuffer[BIGPEMU_MAX_AUDIO_CALLBACK_NP2 >> 1];
static double sSampleDftBuffer[BIGPEMU_MAX_AUDIO_CALLBACK_NP2];
static uint32_t sFirstDftIndex = 0;
static uint32_t sDftSampleCount = 0;
static float sSampleBuckets[FREQ_BUCKET_COUNT];
static float sBucketPeaks[AUDIO_FRAME_HISTORY_SIZE];
static uint32_t sBucketFrameIndex = 0;

#define ROM_PATH_BUFFER_SIZE 1024

#if DEMONSTRATE_RW_HANDLER()
static uint32_t demo_rw_handler(uint32_t *pData, const uint32_t dataAddr, const uint32_t dataSize, const uint32_t rwMask)
{
	if (rwMask & BIGPEMU_HANDLER_MASK_READ)
	{
		printf("Handled read: %08x\n", dataAddr, *pData);
		*pData = 0x666;
	}
	else //if (rwMask & BIGPEMU_HANDLER_MASK_WRITE)
	{
		printf("Prevented write: %08x (%08x)\n", dataAddr, *pData);
	}
	//return 1 if you want to handle the read/write yourself and prevent the underlying handler from running.
	//in this handler, we prevent writes and handle all reads in the range. (this will obviously break butch in our default range)
	return 1;
}
#endif

static uint32_t on_sw_loaded(const int eventHandle, void *pEventData)
{
	sLoadedHash = bigpemu_get_loaded_fnv1a64();
	if (sLastPrintEvents)
	{
		//we could, for example, add some breakpoints here if the hash matches some particular software title we're after
		printf_notify("hello_pussycat loaded event: 0x%08X%08X", (uint32_t)(sLoadedHash >> 32ULL), (uint32_t)(sLoadedHash & 0xFFFFFFFFULL));
		
		char romPath[ROM_PATH_BUFFER_SIZE];
		bigpemu_get_loaded_software_path(romPath, ROM_PATH_BUFFER_SIZE);
		printf("Software path: %s\n", romPath);
	}
	
	memset(sSampleBuckets, 0, sizeof(sSampleBuckets));
	memset(sBucketPeaks, 0, sizeof(sBucketPeaks));
	
#if DEMONSTRATE_RW_HANDLER()
	assert(!(DEMO_START_ADDR % bigpemu_jag_get_rw_handler_alignment()));
	assert(!(DEMO_END_ADDR % bigpemu_jag_get_rw_handler_alignment()));
	//just a simple hook at the end of rom (this is the first available address for custom rw handlers)
	bigpemu_jag_set_rw_handler(demo_rw_handler, DEMO_START_ADDR, DEMO_END_ADDR, (BIGPEMU_HANDLER_MASK_READ | BIGPEMU_HANDLER_MASK_WRITE));
#endif

	return 0;
}

static uint32_t on_sw_unloaded(const int eventHandle, void *pEventData)
{
	sLoadedHash = 0;
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

static uint32_t on_audio_frame(const int eventHandle, void *pEventData)
{
	TBigPEmuAudioFrameParams *pAudioParams = (TBigPEmuAudioFrameParams *)pEventData;
	
	const uint32_t monoCount = (pAudioParams->mSampleCount >> 1);
	bigpemu_audio_stereo_deinterleave_and_expand(sSampleBuffer, NULL, pAudioParams->mpSamples, pAudioParams->mSampleCount);

	const uint32_t maxDftSampleCount = ((monoCount >> 1) + 1);
	const float stepFreq = (float)pAudioParams->mSampleRate / monoCount;
	sFirstDftIndex = (stepFreq > 0.0f) ? BIGPEMU_MIN((uint32_t)(AUDIO_MIN_VIS_FREQUENCY / stepFreq + 0.5f), maxDftSampleCount - 1) : 0;
	
	bigpemu_dft(sSampleDftBuffer, sSampleBuffer, monoCount);

	//if you want to do anything especially fancy here, like a filter which involves non-trivial per-sample operations, you
	//should consider using a call into a native DLL to do your intense processing. see the hello_pussy_dll example below.
	
	const uint32_t usableFreqDomainCount = maxDftSampleCount - sFirstDftIndex;
	bigpemu_quantize_dft_amplitudes(sSampleBuckets, &sSampleDftBuffer[sFirstDftIndex], FREQ_BUCKET_COUNT, usableFreqDomainCount, 0);
	sSampleBuckets[0] *= AUDIO_BUCKET0_SCALE;
	
	//optionally, you could specify (BIGPEMU_DFTQ_NO_SCALING | BIGPEMU_DFTQ_NO_CLAMPING) as flags above and do something like:
	//bigpemu_scale_signal(sSampleBuckets, FREQ_BUCKET_COUNT, 1.0f / monoCount);
	//bigpemu_clamp_signal(sSampleBuckets, FREQ_BUCKET_COUNT, 0.0f, 1.0f);
	//up to aesthetic preference. i'm letting it scale from the provided sample count. (which will saturate often)
	
	const float bucketMax = bigpemu_get_signal_max(sSampleBuckets, FREQ_BUCKET_COUNT);
	sBucketPeaks[sBucketFrameIndex] = bucketMax;
	sBucketFrameIndex = (sBucketFrameIndex + 1) % AUDIO_FRAME_HISTORY_SIZE;
	//normalize from history
	if (bucketMax > 0.0f)
	{
		const float historyMax = bigpemu_get_signal_max(sBucketPeaks, AUDIO_FRAME_HISTORY_SIZE);
		const float f = 1.0f / historyMax;
		bigpemu_scale_signal(sSampleBuckets, FREQ_BUCKET_COUNT, f);
	}

	sDftSampleCount = usableFreqDomainCount;

	//resampling in your callback would go something like:
	/*
	static int16_t sLocalSampleBuffer[BIGPEMU_MAX_AUDIO_CALLBACK_SAMPLES];
	const uint32_t newSampleCount = (uint32_t)(48000.0f / 60.0f + 0.5f) * 2;
	bigpemu_audio_resample(sLocalSampleBuffer, newSampleCount, pAudioParams->mpSamples, pAudioParams->mSampleCount, 2);
	pAudioParams->mpSamples = sLocalSampleBuffer;
	pAudioParams->mSampleCount = newSampleCount;
	*/
	
	return 0;
}

#if DEMONSTRATE_SOUND_PLAYBACK()
static void stop_sound()
{
	if (sSoundId)
	{
		bigpemu_audio_sound_stop(sSoundId);
		sSoundId = 0;
	}	
}
#endif

static void draw_sample_face_image()
{
	//using this system, you could potentially create your own customizable bind interface, and load/store your own configuration using the fs functions.
	//for demo purposes, we've just hardcoded our desired key combo via skHotKeyVKs.
	const uint32_t heldCount = bigpemu_input_get_all_held_inputs(sInputBuffer, sMaxInputsInBuffer);
	if (!heldCount)
	{
#if DEMONSTRATE_SOUND_PLAYBACK()
		stop_sound();
#endif
		return;
	}

	//construct the buffer for our test key combo (accounting for possible changes to the input data format)
	const uint32_t inputDataVer = bigpemu_input_get_input_data_version();
	if (sHotKeyDataVersion != inputDataVer)
	{
		sHotKeyDataVersion = inputDataVer;
		for (uint32_t hkIndex = 0; hkIndex < BIGPEMU_ARRAY_LENGTH(skHotKeyVKs); ++hkIndex)
		{
			bigpemu_input_create_input_from_vk(&sHotKeyInputBuffer[hkIndex * sInputSize], skHotKeyVKs[hkIndex]);
		}
	}
	
	//make sure all the desired keys are held
	for (uint32_t hkIndex = 0; hkIndex < BIGPEMU_ARRAY_LENGTH(skHotKeyVKs); ++hkIndex)
	{
		if (!bigpemu_input_input_in_set(sInputBuffer, heldCount, &sHotKeyInputBuffer[hkIndex * sInputSize]))
		{
#if DEMONSTRATE_SOUND_PLAYBACK()
			stop_sound();
#endif
			return;
		}
	}
	
#if DEMONSTRATE_SOUND_PLAYBACK()
	if (!sSoundRes)
	{
		const char *pSoundName = "hello_pussycat_sample.mp3";
		const uint64_t soundFile = fs_open(pSoundName, 0);
		if (soundFile)
		{
			const uint32_t fileSize = (uint32_t)fs_get_size(soundFile);
			void *pSoundBuffer = bigpemu_vm_alloc(fileSize);
			fs_read(pSoundBuffer, fileSize, soundFile);
			fs_close(soundFile);
			
			sSoundRes = bigpemu_res_sound_from_mp3(bigpemu_get_module_handle(), pSoundBuffer, fileSize);
			bigpemu_vm_free(pSoundBuffer);
		}
		else
		{
			printf("Sound demo requires a file named %s to be placed in the script directory.\n");
		}
	}
	
	if (sSoundRes)
	{
		TBigPEmuSpatialAudio s;
		bigpemu_audio_default_spatial(&s);
		
		const float listenPos[3] = { 0.0f, 0.0f, 0.0f };
		const float listenFwd[3] = { 1.0f, 0.0f, 0.0f };
		const float listenRight[3] = { 0.0f, 1.0f, 0.0f };
		const float listenUp[3] = { 0.0f, 0.0f, 1.0f };
		bigpemu_audio_update_listener(listenPos, listenFwd, listenRight, listenUp);
		
		const double t = bigpemu_time_ms();
		const double ss = t * 0.001;
		
		const float audioSourceDist = 1.0f;
		
		s.mPos[0] = (float)sin(ss) * audioSourceDist;
		s.mPos[1] = (float)cos(ss) * audioSourceDist;
		s.mMaxDist = 7.0f;
		s.mHrir = bigpemu_audio_loadir(NULL, 0); //don't make this call more than once if you're passing a buffer, but it's fine for querying the default hrir
		
		if (!sSoundId || bigpemu_audio_sound_finished(sSoundId))
		{
			sSoundId = bigpemu_audio_play_sound_ex(sSoundRes, 0, &s);
		}
		else
		{
			bigpemu_audio_sound_update_spatial(sSoundId, &s);
		}
	}
#endif
	
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
}

static uint32_t on_post_ui(const int eventHandle, void *pEventData)
{
	//sample code to draw a rectangle over the software display area
	/*
	float dispRect[4];
	bigpemu_drawui_get_virtual_display_rect(dispRect);
	const float screenRectColor[4] = { 0.0f, 1.0f, 0.0f, 0.25f };
	bigpemu_drawui_rect(NULL, dispRect[0], dispRect[2], dispRect[1] - dispRect[0], dispRect[3] - dispRect[2], screenRectColor, NULL);
	*/

	draw_sample_face_image();

	const float specColorA[4] = { 0.0f, 1.0f, 0.0f, 1.0f };
	const float specColorB[4] = { 1.0f, 0.0f, 0.0f, 1.0f };
	
#if USE_SPECTRUM_TEXTURE()
	static float sFullAmplitudeBuffer[BIGPEMU_MAX_AUDIO_CALLBACK_SAMPLES];
	static float sAmpPeaks[AUDIO_FRAME_HISTORY_SIZE];
	static uint32_t sAmpFrameIndex = 0;

	#define SPEC_TEX_WIDTH 1024
	#define SPEC_TEX_HEIGHT 64
	static uint8_t specTexBuffer[SPEC_TEX_WIDTH * SPEC_TEX_HEIGHT * 4];
	const uint32_t texFlags = (BIGPEMU_TEXFLAG_BILINEAR | BIGPEMU_TEXFLAG_GENMIPS | BIGPEMU_TEXFLAG_CLAMP_TO_EDGE);
	
	bigpemu_quantize_dft_amplitudes(sFullAmplitudeBuffer, &sSampleDftBuffer[sFirstDftIndex], sDftSampleCount, sDftSampleCount, BIGPEMU_DFTQ_SCALE_SQRT | BIGPEMU_DFTQ_CLAMP_RENORM);
	const float ampMax = bigpemu_get_signal_max(sFullAmplitudeBuffer, sDftSampleCount);
	sAmpPeaks[sAmpFrameIndex] = ampMax;
	sAmpFrameIndex = (sAmpFrameIndex + 1) % AUDIO_FRAME_HISTORY_SIZE;
	if (ampMax > 0.0f)
	{
		const float historyMax = bigpemu_get_signal_max(sAmpPeaks, AUDIO_FRAME_HISTORY_SIZE);
		const float f = 1.0f / historyMax;
		bigpemu_scale_signal(sFullAmplitudeBuffer, sDftSampleCount, f);
	}
	
	//as with general signal processing, if you want to implement your own routine along these lines, consider a native call.
	//you can probably still get away with doing it in here, but burning that many cycles in the interpreter is fairly sinful.
	bigpemu_render_signal_rgba32(specTexBuffer, SPEC_TEX_WIDTH, SPEC_TEX_HEIGHT, sFullAmplitudeBuffer, sDftSampleCount, specColorA, specColorB);
	
	void *pMod = bigpemu_get_module_handle();
		
	sSpecTexRes = bigpemu_res_texture_rgba32_create_or_reupload(sSpecTexRes, pMod, specTexBuffer, SPEC_TEX_WIDTH, SPEC_TEX_HEIGHT, texFlags);		
	if (sSpecTexRes)
	{
		bigpemu_drawui_rect(sSpecTexRes, 8.0f, 8.0f, 256.0f, 64.0f, NULL, NULL);
	}
#else
	//show our dft buckets
	const float baseBarWidth = 8.0f;
	const float baseBarSpace = 2.0f;
	const float baseBarHeight = 32.0f;
	float drawBarX = 8.0f;
	const float baseBarY = 8.0f + baseBarHeight;
	for (uint32_t bucketIndex = 0; bucketIndex < FREQ_BUCKET_COUNT; ++bucketIndex)
	{
		const float f = BIGPEMU_MAX(sSampleBuckets[bucketIndex], 0.01f);
		const float specColorMod[4] =
		{
			specColorA[0] + (specColorB[0] - specColorA[0]) * f,
			specColorA[1] + (specColorB[1] - specColorA[1]) * f,
			specColorA[2] + (specColorB[2] - specColorA[2]) * f,
			specColorA[3] + (specColorB[3] - specColorA[3]) * f
		};
		const float drawBarHeight = baseBarHeight * f;
		bigpemu_drawui_rect(NULL, drawBarX, baseBarY - drawBarHeight, baseBarWidth, drawBarHeight, specColorMod, specColorA);
		drawBarX += (baseBarWidth + baseBarSpace);
	}
#endif
	
	//update a named variable with the mean just for demo purposes (the liz screen effect references this to show script-effect interaction)
	const float meanBucket = -bigpemu_get_signal_mean(sSampleBuckets, FREQ_BUCKET_COUNT);
	bigpemu_set_named_var_data("sLizScriptBasedBias", &meanBucket, sizeof(meanBucket));
	
	return 0;
}

#if DEMONSTRATE_RW_HANDLER()
static uint32_t on_emu_frame(const int eventHandle, void *pEventData)
{
	//demonstrate the write being prevented
	bigpemu_jag_write32(DEMO_START_ADDR, 1);
	
	//demonstrate the read being handled
	const uint32_t readData = bigpemu_jag_read32(DEMO_START_ADDR);
	printf("Read back: %08x\n", readData);
	
	return 0;
}
#endif

void bigp_init()
{
	void *pMod = bigpemu_get_module_handle();
	
	const int catHandle = bigpemu_register_setting_category(pMod, "Hello Pussycat");
	sPrintEventsSettingHandle = bigpemu_register_setting_bool(catHandle, "Print Events", 1);
	
	sOnLoadEvent = bigpemu_register_event_sw_loaded(pMod, on_sw_loaded);
	sOnUnloadEvent = bigpemu_register_event_sw_unloaded(pMod, on_sw_unloaded);
	sOnVideoEvent = bigpemu_register_event_video_thread_frame(pMod, on_video_frame);
	sOnPostUIEvent = bigpemu_register_event_post_ui(pMod, on_post_ui);
#if DEMONSTRATE_RW_HANDLER()
	sOnEmuThreadFrameEvent = bigpemu_register_event_emu_thread_frame(pMod, on_emu_frame);
#endif
	
	sInputSize = bigpemu_input_get_input_size();
	sMaxInputsInBuffer = BIGPEMU_MIN(INPUT_BUFFER_SIZE / sInputSize, 8);
	assert(sMaxInputsInBuffer >= BIGPEMU_ARRAY_LENGTH(skHotKeyVKs));
	
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

	sLoadedHash = 0;
	
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
	
	sOnAudioFrameEvent = bigpemu_register_event_audio_frame(pMod, on_audio_frame);
}

void bigp_shutdown()
{
	void *pMod = bigpemu_get_module_handle();
	//not strictly necessary since these events will be destroyed with the module, but we're establishing good practices here.
	bigpemu_unregister_event(pMod, sOnLoadEvent);
	bigpemu_unregister_event(pMod, sOnUnloadEvent);
	bigpemu_unregister_event(pMod, sOnVideoEvent);
	bigpemu_unregister_event(pMod, sOnPostUIEvent);
	bigpemu_unregister_event(pMod, sOnAudioFrameEvent);
#if DEMONSTRATE_RW_HANDLER()
	bigpemu_unregister_event(pMod, sOnEmuThreadFrameEvent);
	sOnEmuThreadFrameEvent = -1;
#endif
	sOnLoadEvent = -1;
	sOnUnloadEvent = -1;
	sOnVideoEvent = -1;
	sOnPostUIEvent = -1;
	sOnAudioFrameEvent = -1;
	sPrintEventsSettingHandle = -1;
	sLastPrintEvents = 0;
	sHotKeyDataVersion = BIGPEMU_INPUT_DATA_VERSION_INVALID;
	if (sPicTexRes)
	{
		bigpemu_res_texture_free(pMod, sPicTexRes);
		sPicTexRes = 0;
	}
#if USE_SPECTRUM_TEXTURE()
	if (sSpecTexRes)
	{
		bigpemu_res_texture_free(pMod, sSpecTexRes);
		sSpecTexRes = 0;
	}
#endif
#if DEMONSTRATE_SOUND_PLAYBACK()
	sSoundId = 0;
	if (sSoundRes)
	{
		bigpemu_res_sound_free(pMod, sSoundRes);
		sSoundRes = 0;
	}
#endif

	printf("hello_pussycat shutdown\n");
}
