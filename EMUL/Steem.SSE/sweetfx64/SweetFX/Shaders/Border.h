  /*-----------------------------------------------------------.
 /                          Border                            /
'-----------------------------------------------------------*/
// Version 1.1

/*
Version 1.0 by Oomek
- Fixes light, one pixel thick border in some games when forcing MSAA like i.e. Dishonored

Version 1.1 by CeeJay.dk
- Optimized the shader. It still does the same but now it runs faster.
*/


float4 BorderPass( float4 colorInput, float2 tex )
{

float2 distance = abs(tex - 0.5); //calculate distance from center

//bool2 screen_border = step(distance,0.5 - pixel); //is the distance less than the max - 1 pixel?
bool2 screen_border = step(0.5 - pixel,distance); //is the distance greater than the max - 1 pixel?

colorInput.rgb = (!dot(screen_border, 1.0)) ? colorInput.rgb : 0.0; //if neither x or y is greater then do nothing, but if one them is greater then set the color to black.
//colorInput.rgb = saturate(colorInput.rgb - dot(screen_border, 1.0));
//colorInput.rgb = colorInput.rgb * screen_border.x - !screen_border.y;

return colorInput; //return the pixel

} 