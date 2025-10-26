   /*-----------------------------------------------------------.   
  /                           Dither                            /
  '-----------------------------------------------------------*/
/* 
  Dither version 1.2.0
  by Christian Cann Schuldt Jensen ~ CeeJay.dk
  
  Does dithering of the greather than 8-bit per channel precision used in shaders.
  Even halfs offer 10(+1)bit per channel
*/

float4 DitherPass( float4 colorInput, float2 tex )
{
   float3 color = colorInput.rgb;
   
   //color = (tex.x / 2.0); //draw a gradient for testing.
   
   float dither_size = 2.0;  //Size of the dithering grid - I'm using a 2x2 grid here.
   float dither_bit  = 8.0;  //Number of bits per channel. Should be 8 for most monitors.

   #if dither_method == 2 //random dithering

     //make some noise
     float noise = frac(sin(dot(float4(tex,-tex.yx), float4(float2(12.9898,78.233),float2(12.9898,78.233)* acos(-1)))) * 43758.5453); //pseudo random number generator
     noise -= 0.5;
     
     /*
     //Calculate grid position
     float grid_position = frac(dot(tex,(screen_size / dither_size)) + (0.5 / dither_size)); //returns 0.25 and 0.75
          
     //modify noise for extra dithering
     noise = lerp(2.0 * noise, -2.0 * noise, grid_position); //do i really need to use the gridposition with noise? - signs point to no.
     */

     //Calculate how big the shift should be
     float dither_shift = (noise) * (1.0 / (pow(2,dither_bit) - 1.0)); // using noise to determine shift. The noise ahould vary between +- 0.5.
                                                                       
     //Shift the individual colors differently, thus making it even harder to see the dithering pattern
     float3 dither_shift_RGB = float3(-dither_shift, dither_shift, -dither_shift); //subpixel dithering
     
     //shift the color by dither_shift    
     //dither_shift_RGB = lerp(2.0 * dither_shift_RGB, -2.0 * dither_shift_RGB, grid_position); //do i really need to use the gridposition with noise?
     color.rgb += dither_shift_RGB;
     
   #else //dither_method == 1 , Ordered dithering
     //Calculate grid position
     float grid_position = frac(dot(tex,(screen_size / dither_size)) + (0.5 / dither_size)); //returns 0.25 and 0.75
   
     //Calculate how big the shift should be
     float dither_shift = (0.25) * (1.0 / (pow(2,dither_bit) - 1.0)); // 0.25 seems good both when using math and when eyeballing it. So does 0.75 btw.

     //Shift the individual colors differently, thus making it even harder to see the dithering pattern
     float3 dither_shift_RGB = float3(dither_shift, -dither_shift, dither_shift); //subpixel dithering

     //modify shift acording to grid position.
     dither_shift_RGB = lerp(2.0 * dither_shift_RGB, -2.0 * dither_shift_RGB, grid_position); //shift acording to grid position.
     
     //shift the color by dither_shift
     //color.rgb += lerp(2.0 * dither_shift_RGB, -2.0 * dither_shift_RGB, grid_position); //shift acording to grid position.
     color.rgb += dither_shift_RGB;
   #endif

   //colorInput.rgb = (dither_shift_RGB * 2.0 * (pow(2,dither_bit) - 1.0) ) + 0.5; //visualize the shift
   //colorInput.rgb = grid_position; //visualize the grid
   //colorInput.rgb = noise; //visualize the noise
   
   colorInput.rgb = color.rgb;
   
   return colorInput;
}