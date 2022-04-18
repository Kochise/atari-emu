   /*-----------------------------------------------------------.   
  /                          Vignette                           /
  '-----------------------------------------------------------*/
/*
  Version 1.2
  
  Darkens the edges of the image to make it look more like it was shot with a camera lens.
  May cause banding artifacts.
*/
#ifndef VignetteRatio
  #define VignetteRatio 1.0
#endif

//XOR - not used right now but it might be useful at a later time
float XOR( float xor_A, float xor_B )
{
  return saturate( dot(float4(-xor_A ,-xor_A ,xor_A , xor_B) , float4(xor_B, xor_B ,1.0 ,1.0 ) ) ); // (-A * B) + (-A * B) + (A * 1.0) + (B * 1.0)
}

float4 VignettePass( float4 colorInput, float2 tex )
{
	float3 vignette = colorInput.rgb;
	
	//Set the center
	float2 tc = tex - VignetteCenter;
	
	//Make the ratio 1:1
	tc *= float2((pixel.y / pixel.x),VignetteRatio);
	 
	//Calculate the distance
    float v = length(tc) / VignetteRadius;
    
    //Apply the vignette
	colorInput.rgb = vignette * (1.0 + pow(v, VignetteSlope) * VignetteAmount); //pow - multiply
	
	return colorInput;
}

/* CRT style vignette - needs work.
tex = abs(tex -0.5);
float tc2 = XOR(tex.x,tex.y);//1-dot(tex.x,tex.y)*4;
tc2 = saturate(tc2 -0.49);
colorInput.rgb = colorInput.rgb * (1-tc2*100); //or maybe abs(tc2*100-1)
*/