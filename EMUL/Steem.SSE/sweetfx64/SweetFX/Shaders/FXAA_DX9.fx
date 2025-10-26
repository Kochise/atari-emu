texture2D frameTex2D;

#define FXAA_PC 1
#define FXAA_HLSL_3 1

#include "..\..\SweetFX_preset.txt"
#include "FXAA.h"

sampler frameSampler = sampler_state
{
	texture = <frameTex2D>;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
    MipFilter = NONE;
    AddressU = BORDER;
    AddressV = BORDER;
    SRGBTexture = FALSE;
};

struct VSOUT
{
	float4 vertPos : POSITION;
	float2 UVCoord : TEXCOORD0;
};

struct VSIN
{
	float4 vertPos : POSITION0;
	float2 UVCoord : TEXCOORD0;
};

VSOUT FrameVS(VSIN IN)
{
	VSOUT OUT;
	OUT.vertPos = IN.vertPos;
	OUT.UVCoord = IN.UVCoord;
	return OUT;
}

//Include the main SweetFX control shader
#include "Main.h"

float4 calcLuma(VSOUT IN) : COLOR0
{
	float4 color = tex2D(frameSampler, IN.UVCoord);
	color.a = dot(color.rgb, float3(0.299, 0.587, 0.114)); 
	//return float4(color.a,color.a,color.a,color.a);
	return color;
}

float4 applyFXAA(VSOUT IN) : COLOR0
{
	float4 c0 = FxaaPixelShader(IN.UVCoord, float4(0,0,0,0), frameSampler, frameSampler,
				frameSampler, PIXEL_SIZE, float4(0,0,0,0), float4(0,0,0,0), float4(0,0,0,0),
				//
				// Only used on FXAA Quality.
				// This used to be the FXAA_QUALITY__SUBPIX define.
				// It is here now to allow easier tuning.
				// Choose the amount of sub-pixel aliasing removal.
				// This can effect sharpness.
				//   1.00 - upper limit (softer)
				//   0.75 - default amount of filtering
				//   0.50 - lower limit (sharper, less sub-pixel aliasing removal)
				//   0.25 - almost off
				//   0.00 - completely off
				fxaa_Subpix,
				//
				// Only used on FXAA Quality.
				// This used to be the FXAA_QUALITY__EDGE_THRESHOLD define.
				// It is here now to allow easier tuning.
				// The minimum amount of local contrast required to apply algorithm.
				//   0.333 - too little (faster)
				//   0.250 - low quality
				//   0.166 - default
				//   0.125 - high quality 
				//   0.063 - overkill (slower)
				fxaa_EdgeThreshold,
				//
				// Only used on FXAA Quality.
				// This used to be the FXAA_QUALITY__EDGE_THRESHOLD_MIN define.
				// It is here now to allow easier tuning.
				// Trims the algorithm from processing darks.
				//   0.0833 - upper limit (default, the start of visible unfiltered edges)
				//   0.0625 - high quality (faster)
				//   0.0312 - visible limit (slower)
				// Special notes when using FXAA_GREEN_AS_LUMA,
				//   Likely want to set this to zero.
				//   As colors that are mostly not-green
				//   will appear very dark in the green channel!
				//   Tune by looking at mostly non-green content,
				//   then start at zero and increase until aliasing is a problem.
				fxaa_EdgeThresholdMin,
				8.0, 0.125, 0.05, float4(0,0,0,0) );
	c0.a = 1;
	c0 = main(IN.UVCoord, c0); // Add the other effects
	return c0;
}

technique t0
{
	pass P0
	{
		VertexShader = compile vs_3_0 FrameVS();
		PixelShader = compile ps_3_0 calcLuma();  
		ZEnable = false;        
		SRGBWriteEnable = false;
		AlphaBlendEnable = false;
		AlphaTestEnable = false;
		ColorWriteEnable = RED|GREEN|BLUE|ALPHA;
	}

	pass P1
	{
		VertexShader = compile vs_3_0 FrameVS();
		PixelShader = compile ps_3_0 applyFXAA();     
		ZEnable = false;        
		SRGBWriteEnable = false;
		AlphaBlendEnable = false;
		AlphaTestEnable = false;
	}
}