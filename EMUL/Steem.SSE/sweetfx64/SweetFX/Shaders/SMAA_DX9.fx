/**
 * Copyright (C) 2011 Jorge Jimenez (jorge@iryoku.com)
 * Copyright (C) 2011 Belen Masia (bmasia@unizar.es) 
 * Copyright (C) 2011 Jose I. Echevarria (joseignacioechevarria@gmail.com) 
 * Copyright (C) 2011 Fernando Navarro (fernandn@microsoft.com) 
 * Copyright (C) 2011 Diego Gutierrez (diegog@unizar.es)
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 *    1. Redistributions of source code must retain the above copyright notice,
 *       this list of conditions and the following disclaimer.
 * 
 *    2. Redistributions in binary form must reproduce the following disclaimer
 *       in the documentation and/or other materials provided with the 
 *       distribution:
 * 
 *      "Uses SMAA. Copyright (C) 2011 by Jorge Jimenez, Jose I. Echevarria,
 *       Belen Masia, Fernando Navarro and Diego Gutierrez."
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS 
 * IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, 
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR 
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDERS OR CONTRIBUTORS 
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
 * POSSIBILITY OF SUCH DAMAGE.
 * 
 * The views and conclusions contained in the software and documentation are 
 * those of the authors and should not be interpreted as representing official
 * policies, either expressed or implied, of the copyright holders.
 */

#include "..\..\SweetFX_preset.txt"

/**
 * Setup mandatory defines. Use a real macro here for maximum performance!
 */
#ifndef SMAA_PIXEL_SIZE // It's actually set on runtime, this is for compilation time syntax checking.
#define SMAA_PIXEL_SIZE float2(1.0 / 1280.0, 1.0 / 720.0)
#endif

/**
 * This can be ignored; its purpose is to support interactive custom parameter
 * tweaking.
 */

/*
float threshold;
float maxSearchSteps;
float maxSearchStepsDiag;
*/

/*
#ifdef SMAA_PRESET_CUSTOM
#define SMAA_THRESHOLD threshold
#define SMAA_MAX_SEARCH_STEPS maxSearchSteps
#define SMAA_MAX_SEARCH_STEPS_DIAG maxSearchStepsDiag
#define SMAA_FORCE_DIAGONALS 1
#endif
*/

// Set the HLSL version:
//#define SMAA_HLSL_3 1

// And include our header!
#include "SMAA.h"

/**
 * Input vars and textures.
 */

texture2D colorTex2D;
/*texture2D depthTex2D;*/
texture2D edgesTex2D;
texture2D blendTex2D;
texture2D areaTex2D;
texture2D searchTex2D;



/**
 * DX9 samplers.
 */
sampler2D colorTex {
    Texture = <colorTex2D>;
    AddressU  = Clamp; AddressV = Clamp;
    MipFilter = Point; MinFilter = Linear; MagFilter = Linear;
    SRGBTexture = true;
};

sampler2D colorTexG {
    Texture = <colorTex2D>;
    AddressU  = Clamp; AddressV = Clamp;
    MipFilter = Linear; MinFilter = Linear; MagFilter = Linear;
    SRGBTexture = false;
};

/*
sampler2D depthTex {
    Texture = <depthTex2D>;
    AddressU  = Clamp; AddressV = Clamp;
    MipFilter = Linear; MinFilter = Linear; MagFilter = Linear;
    SRGBTexture = false;
};
*/

sampler2D edgesTex {
    Texture = <edgesTex2D>;
    AddressU = Clamp; AddressV = Clamp;
    MipFilter = Linear; MinFilter = Linear; MagFilter = Linear;
    SRGBTexture = false;
};

sampler2D blendTex {
    Texture = <blendTex2D>;
    AddressU = Clamp; AddressV = Clamp;
    MipFilter = Linear; MinFilter = Linear; MagFilter = Linear;
    SRGBTexture = false;
};

sampler2D areaTex {
    Texture = <areaTex2D>;
    AddressU = Clamp; AddressV = Clamp; AddressW = Clamp;
    MipFilter = Linear; MinFilter = Linear; MagFilter = Linear;
    SRGBTexture = false;
};

sampler2D searchTex {
    Texture = <searchTex2D>;
    AddressU = Clamp; AddressV = Clamp; AddressW = Clamp;
    MipFilter = Point; MinFilter = Point; MagFilter = Point;
    SRGBTexture = false;
};

#include "Main.h"

/**
 * Function wrappers
 */
void DX9_SMAAEdgeDetectionVS(inout float4 position : POSITION,
                             inout float2 texcoord : TEXCOORD0,
                             out float4 offset[3] : TEXCOORD1) {
    SMAAEdgeDetectionVS(position, position, texcoord, offset);
}

void DX9_SMAABlendingWeightCalculationVS(inout float4 position : POSITION,
                                         inout float2 texcoord : TEXCOORD0,
                                         out float2 pixcoord : TEXCOORD1,
                                         out float4 offset[3] : TEXCOORD2) {
    SMAABlendingWeightCalculationVS(position, position, texcoord, pixcoord, offset);
}

void DX9_SMAANeighborhoodBlendingVS(inout float4 position : POSITION,
                                    inout float2 texcoord : TEXCOORD0,
                                    out float4 offset[2] : TEXCOORD1) {
    SMAANeighborhoodBlendingVS(position, position, texcoord, offset);
}


float4 DX9_SMAALumaEdgeDetectionPS(float4 position : SV_POSITION,
                                   float2 texcoord : TEXCOORD0,
                                   float4 offset[3] : TEXCOORD1,
                                   uniform SMAATexture2D colorGammaTex) : COLOR {
    return SMAALumaEdgeDetectionPS(texcoord, offset, colorGammaTex);
}

float4 DX9_SMAAColorEdgeDetectionPS(float4 position : SV_POSITION,
                                    float2 texcoord : TEXCOORD0,
                                    float4 offset[3] : TEXCOORD1,
                                    uniform SMAATexture2D colorGammaTex) : COLOR {
    return SMAAColorEdgeDetectionPS(texcoord, offset, colorGammaTex);
}

float4 DX9_SMAADepthEdgeDetectionPS(float4 position : SV_POSITION,
                                    float2 texcoord : TEXCOORD0,
                                    float4 offset[3] : TEXCOORD1,
                                    uniform SMAATexture2D depthTex) : COLOR {
    return SMAADepthEdgeDetectionPS(texcoord, offset, depthTex);
}

float4 DX9_SMAABlendingWeightCalculationPS(float4 position : SV_POSITION,
                                           float2 texcoord : TEXCOORD0,
                                           float2 pixcoord : TEXCOORD1,
                                           float4 offset[3] : TEXCOORD2,
                                           uniform SMAATexture2D edgesTex, 
                                           uniform SMAATexture2D areaTex, 
                                           uniform SMAATexture2D searchTex) : COLOR {
    return SMAABlendingWeightCalculationPS(texcoord, pixcoord, offset, edgesTex, areaTex, searchTex, 0);
}

float4 DX9_SMAANeighborhoodBlendingPS(float4 position : SV_POSITION,
                                      float2 texcoord : TEXCOORD0,
                                      float4 offset[2] : TEXCOORD1,
                                      uniform SMAATexture2D colorTex,
                                      uniform SMAATexture2D blendTex) : COLOR {
    return SMAANeighborhoodBlendingPS(texcoord, offset, colorTex, blendTex); //Returning the pixel
}

float4 DX9_FinalGammaPassPS(float4 position : SV_POSITION,
                            float2 texcoord : TEXCOORD0,
                            uniform SMAATexture2D colorTexG) : COLOR {
    return main(texcoord, tex2D(colorTexG, texcoord));
}

   /*-----------------------------------------------------------.   
  /                SweetFX - SMAA_off function                  /
  '-----------------------------------------------------------*/

/**
 * SMAA off Vertex Shader
 */
void SMAA_off_VS(float4 position,
                 out float4 svPosition,
                 inout float2 texcoord) {
    svPosition = position;
}

void DX9_SMAA_off_VS(inout float4 position : POSITION,
                     inout float2 texcoord : TEXCOORD0) {
    //SMAA_off_VS(position, position, texcoord);
}

float4 DX9_SMAA_off_PS( float2 texcoord : TEXCOORD0) : COLOR {
                                      
    float4 SMAAoutput = tex2D(s0, texcoord);

    SMAAoutput = main(texcoord,SMAAoutput); // Add the other effects
    
    return SMAAoutput;  //Returning the pixel
}

   /*----------------------------------------------------------*/   
  
#if (USE_SMAA_ANTIALIASING == 1)

/**
 * Time for some techniques!
 */
technique LumaEdgeDetection { // Pass 1A
    pass LumaEdgeDetection {
        VertexShader = compile vs_3_0 DX9_SMAAEdgeDetectionVS();
        PixelShader = compile ps_3_0 DX9_SMAALumaEdgeDetectionPS(colorTexG);
   
        SRGBWriteEnable = false;
        AlphaBlendEnable = false;

        // We will be creating the stencil buffer for later usage.
        StencilEnable = true;
        StencilPass = REPLACE;
        StencilRef = 1;
    }
}

technique ColorEdgeDetection { // Pass 1B
    pass ColorEdgeDetection {
        VertexShader = compile vs_3_0 DX9_SMAAEdgeDetectionVS();
        PixelShader = compile ps_3_0 DX9_SMAAColorEdgeDetectionPS(colorTexG);
   
        SRGBWriteEnable = false;
        AlphaBlendEnable = false;

        // We will be creating the stencil buffer for later usage.
        StencilEnable = true;
        StencilPass = REPLACE;
        StencilRef = 1;
    }
}

/*
technique DepthEdgeDetection { // Pass 1C
    pass DepthEdgeDetection {
        VertexShader = compile vs_3_0 DX9_SMAAEdgeDetectionVS();
        PixelShader = compile ps_3_0 DX9_SMAADepthEdgeDetectionPS(depthTex);

        SRGBWriteEnable = false;
        AlphaBlendEnable = false;

        // We will be creating the stencil buffer for later usage.
        StencilEnable = true;
        StencilPass = REPLACE;
        StencilRef = 1;
    }
}
*/

technique BlendWeightCalculation { // Pass 2
    pass BlendWeightCalculation {
        VertexShader = compile vs_3_0 DX9_SMAABlendingWeightCalculationVS();
        PixelShader = compile ps_3_0 DX9_SMAABlendingWeightCalculationPS(edgesTex, areaTex, searchTex);

        SRGBWriteEnable = false;
        AlphaBlendEnable = false;

        // Here we want to process only marked pixels.
        StencilEnable = true;
        StencilPass = KEEP;
        StencilFunc = EQUAL;
        StencilRef = 1;
    }
}

technique NeighborhoodBlending { // Pass 3
    pass NeighborhoodBlending {
        VertexShader = compile vs_3_0 DX9_SMAANeighborhoodBlendingVS();
        PixelShader = compile ps_3_0 DX9_SMAANeighborhoodBlendingPS(colorTex, blendTex);

        SRGBWriteEnable = true;
        AlphaBlendEnable = false;

        // Here we want to process all the pixels.
        StencilEnable = false;
    }
}

technique FinalGammaPass { // Pass 4
    pass FinalGammaPass {
        VertexShader = compile vs_3_0 DX9_SMAA_off_VS();
        PixelShader = compile ps_3_0 DX9_FinalGammaPassPS(colorTexG);

        SRGBWriteEnable = false;
        AlphaBlendEnable = false;

        // Here we want to process all the pixels.
        StencilEnable = false;
    }
}

#else
   // if SMAA is off
technique SMAA_off { // Not doing SMAA.
    pass SMAA_off {
        VertexShader = compile vs_3_0 DX9_SMAA_off_VS();
        PixelShader = compile ps_3_0 DX9_SMAA_off_PS(); //Use this with GPU Shaderanalyzer

        SRGBWriteEnable = false;
        AlphaBlendEnable = false;

        // Here we want to process all the pixels.
        StencilEnable = false;
    }
}
#endif
