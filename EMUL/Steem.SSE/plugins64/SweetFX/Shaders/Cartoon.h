/*------------------------------------------------------------------------------
						Cartoon
------------------------------------------------------------------------------*/

float4 CartoonPass( float4 colorInput, float2 Tex )
{
  float3 CoefLuma2 = float3(0.2126, 0.7152, 0.0722);  //Values to calculate luma with
  
  float diff1 = dot(CoefLuma2,myTex2D(s0, Tex + pixel).rgb);
  diff1 -= dot(CoefLuma2,myTex2D(s0, Tex - pixel).rgb);
  
  float diff2 = dot(CoefLuma2,myTex2D(s0, Tex +float2(pixel.x,-pixel.y)).rgb);
  diff2 -= dot(CoefLuma2,myTex2D(s0, Tex +float2(-pixel.x,pixel.y)).rgb);
    
  float edge = dot(float2(diff1,diff2),float2(diff1,diff2));
  
  colorInput.rgb = colorInput.rgb - edge * CartoonPower;
	
  return saturate(colorInput);
}
