Texture2D frameTex2D;

#define FXAA_PC 1

// Set pixel shader version accordingly:
#if FXAA_HLSL_5 == 1
#define VS_VERSION vs_5_0
#define PS_VERSION ps_5_0
#else
#define VS_VERSION vs_4_0
#define PS_VERSION ps_4_0
#endif

#include "..\..\SweetFX_preset.txt"
#include "FXAA.h"

SamplerState LinearSampler {
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;
    AddressV = Wrap;
};

DepthStencilState DisableDepthStencil {
    DepthEnable = FALSE;
    StencilEnable = FALSE;
	DepthWriteMask = ZERO;
};

BlendState NoBlending {
    AlphaToCoverageEnable = FALSE;
    BlendEnable[0] = FALSE;
    SrcBlend = SRC_COLOR;
    DestBlend = ONE;
    BlendOp = ADD;
    SrcBlendAlpha = ZERO;
    DestBlendAlpha = ZERO;
    BlendOpAlpha = ADD;
    RenderTargetWriteMask[0] = 0x0F;
};

struct VSOUT
{
	float4 vertPos : SV_POSITION;
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

float4 calcLuma(VSOUT IN) : SV_TARGET
{
	float4 color = frameTex2D.SampleLevel(LinearSampler, IN.UVCoord, 0);
	color.a = dot(color.rgb, float3(0.299, 0.587, 0.114));
	//color.w = sqrt(dot(color.xyz,float3(0.299, 0.587, 0.114)));
	//return float4(color.a,color.a,color.a,color.a);
	return color;
}

float4 applyFXAA(VSOUT IN) : SV_TARGET
{
    FxaaTex fxaaTex = { LinearSampler, frameTex2D };
	float4 c0 = FxaaPixelShader(IN.UVCoord, 
								float4(0,0,0,0),
                                fxaaTex,
                                fxaaTex,
								fxaaTex,
								PIXEL_SIZE, float4(0,0,0,0), float4(0,0,0,0), float4(0,0,0,0),
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

technique10 FxaaTechnique
{
	pass P0
	{
        SetVertexShader(CompileShader(VS_VERSION, FrameVS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(PS_VERSION, calcLuma()));
        SetDepthStencilState(DisableDepthStencil, 0);
        SetBlendState(NoBlending, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
	}

	pass P1
	{
        SetVertexShader(CompileShader(VS_VERSION, FrameVS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(PS_VERSION, applyFXAA()));   
        SetDepthStencilState(DisableDepthStencil, 0);
        SetBlendState(NoBlending, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
	}
}