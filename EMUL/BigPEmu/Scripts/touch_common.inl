#ifndef _TOUCH_COMMON_INL
#define _TOUCH_COMMON_INL

#define MAX_TRACKED_TOUCH_COUNT 10 //apologies to users with 11+ fingers and/or extra-long appendages
static uint32_t sTouchCount = 0;
static uint32_t sPrevTouchCount = 0;
static TBigPEmuTouchInfo sTrackedTouches[MAX_TRACKED_TOUCH_COUNT];
static TBigPEmuTouchInfo sPrevTouches[MAX_TRACKED_TOUCH_COUNT];

typedef struct
{
	float mDelta[2];
	float mMaxDistanceFromInitial;
	uint64_t mBeginFrame;
} TExtraTouchData;
static TExtraTouchData sTouchExtra[MAX_TRACKED_TOUCH_COUNT];

static void update_touches(const uint64_t currentFrame)
{
	if (sTouchCount > 0)
	{
		memcpy(sPrevTouches, sTrackedTouches, sizeof(TBigPEmuTouchInfo) * sTouchCount);
	}
	sPrevTouchCount = sTouchCount;
	
	sTouchCount = BIGPEMU_MIN(bigpemu_touch_count(), MAX_TRACKED_TOUCH_COUNT);
	for (uint32_t touchIndex = 0; touchIndex < sTouchCount; ++touchIndex)
	{
		TBigPEmuTouchInfo *pTch = &sTrackedTouches[touchIndex];
		TExtraTouchData *pEx = &sTouchExtra[touchIndex];
		bigpemu_touch_info(pTch, (int32_t)touchIndex);
		pEx->mDelta[0] = pEx->mDelta[1] = 0.0f;
		//automatically track the deltas
		uint32_t foundPrev = 0;
		for (uint32_t prevIndex = 0; prevIndex < sPrevTouchCount; ++prevIndex)
		{
			TBigPEmuTouchInfo *pPrevTch = &sPrevTouches[prevIndex];
			if (pPrevTch->mId == pTch->mId)
			{
				foundPrev = 1;
				pEx->mDelta[0] = pTch->mPos[0] - pPrevTch->mPos[0];
				pEx->mDelta[1] = pTch->mPos[1] - pPrevTch->mPos[1];
				break;
			}
		}
		if (!foundPrev)
		{
			pEx->mBeginFrame = currentFrame;
			pEx->mMaxDistanceFromInitial = 0.0f;
		}
		const float d[3] = { pTch->mPos[0] - pTch->mInitialPos[0], pTch->mPos[1] - pTch->mInitialPos[1], 0.0f };
		pEx->mMaxDistanceFromInitial = BIGPEMU_MAX(pEx->mMaxDistanceFromInitial, math_vec3_length(d));
	}
}

//should always be called on the emulator thread.
//won't necessarily map nicely to non-standard windows with the per-game scale/bias hacks.
static const uint32_t skTouchPosToJagFlag_FixedEdge = (1 << 0);
static void touch_pos_to_clamped_jag_pos(int32_t *pOut, const float *pPos, const float *pDispRect, const float *pOptionalScaleBias, const uint32_t flags)
{
	const float dispWidth = pDispRect[1] - pDispRect[0];
	const float dispHeight = pDispRect[3] - pDispRect[2];
	const float x = BIGPEMU_MAX(BIGPEMU_MIN(pPos[0], pDispRect[1]), pDispRect[0]);
	const float y = BIGPEMU_MAX(BIGPEMU_MIN(pPos[1], pDispRect[3]), pDispRect[2]);
	int32_t jagRect[4];
	bigpemu_jag_get_display_region(jagRect);
	const int32_t modeDiv = bigpemu_jag_get_vmode_divisor();
	float fX = ((x - pDispRect[0]) / dispWidth);
	float fY = ((y - pDispRect[2]) / dispHeight);
	jagRect[0] /= modeDiv;
	jagRect[1] /= modeDiv;
	if (pOptionalScaleBias)
	{
		fX *= pOptionalScaleBias[0];
		fY *= pOptionalScaleBias[1];
	}
	const int32_t jagX = (flags & skTouchPosToJagFlag_FixedEdge) ? 176 / modeDiv : jagRect[0];
	const int32_t jagY = (flags & skTouchPosToJagFlag_FixedEdge) ? 14 : jagRect[2];
	float fNewPosX = (float)jagX + fX * (jagRect[1] - jagRect[0]);
	float fNewPosY = (float)jagY + fY * (jagRect[3] - jagRect[2]);
	if (pOptionalScaleBias)
	{
		fNewPosX += pOptionalScaleBias[2];
		fNewPosY += pOptionalScaleBias[3];
	}
	pOut[0] = (int32_t)(fNewPosX + 0.5f);
	pOut[1] = (int32_t)(fNewPosY + 0.5f);
}

static void reset_touches()
{
	sPrevTouchCount = sTouchCount = 0;
	memset(sTouchExtra, 0, sizeof(sTouchExtra));
}

#endif //_TOUCH_COMMON_INL
