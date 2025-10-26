   /*-----------------------------------------------------------.   
  /                          Curves                             /
  '-----------------------------------------------------------*/
/*
  by Christian Cann Schuldt Jensen ~ CeeJay.dk
  
  Curves, uses S-curves to increase contrast, without clipping highlights and shadows.
*/

float4 CurvesPass( float4 colorInput )
{
  float3 lumCoeff = float3(0.2126, 0.7152, 0.0722);  //Values to calculate luma with
  float Curves_contrast_blend = Curves_contrast;
  float PI = acos(-1); //3.1415926589

   /*-----------------------------------------------------------.   
  /               Separation of Luma and Chroma                 /
  '-----------------------------------------------------------*/

  // -- Calculate Luma and Chroma if needed --
  #if Curves_mode != 2
  
    //calculate luma (grey)
    float luma = dot(lumCoeff, colorInput.rgb);
	
    //calculate chroma
	  float3 chroma = colorInput.rgb - luma;
  #endif

  // -- Which value to put through the contrast formula? --
  // I name it x because makes it easier to copy-paste to Graphtoy or Wolfram Alpha or another graphing program
  #if Curves_mode == 2
	  float3 x = colorInput.rgb; //if the curve should be applied to both Luma and Chroma
	#elif Curves_mode == 1
	  float3 x = chroma; //if the curve should be applied to Chroma
	  x = x * 0.5 + 0.5; //adjust range of Chroma from -1 -> 1 to 0 -> 1
  #else // Curves_mode == 0
    float x = luma; //if the curve should be applied to Luma
  #endif
	
   /*-----------------------------------------------------------.   
  /                     Contrast formulas                       /
  '-----------------------------------------------------------*/
		
  // -- Curve 1 --
  #if Curves_formula == 1
    x = sin(PI * 0.5 * x); // Sin - 721 amd fps, +vign 536 nv
    x *= x;  
  #endif
  
  // -- Curve 2 --
  #if Curves_formula == 2
    x = ( (x - 0.5) / (0.5 + abs(x-0.5)) ) + 0.5; // 717 amd fps, +vign 519 nv   
  #endif

  // -- Curve 3 --
  #if Curves_formula == 3
    //x = smoothstep(0.0,1.0,x); //smoothstep
    x = x*x*(3.0-2.0*x); //faster smoothstep alternative - 776 amd fps, +vign 536 nv
    //x = x - 2.0 * (x - 1.0) * x* (x- 0.5);  //2.0 is contrast. Range is 0.0 to 2.0
  #endif

  // -- Curve 4 --
  #if Curves_formula == 4
    x = 1.1048 / (1.0 + exp(-3.0 * (x * 2.0 - 1.0))) - (0.1048 / 2.0); //exp formula - 706 amd fps , +vign 511 nv
  #endif

  // -- Curve 5 --
  #if Curves_formula == 5
    //x = 0.5 * (x + 3.0 * x * x - 2.0 * x * x * x); //a simplified catmull-rom (0,0,1,1) - btw smoothstep can also be expressed as a simplified catmull-rom using (1,0,1,0)
    //x = (0.5 * x) + (1.5 -x) * x*x; //estrin form - faster version
    x = x * (x * (1.5-x) + 0.5); //horner form - fastest version
  
    Curves_contrast_blend = Curves_contrast * 2.0; //I multiply by two to give it a strength closer to the other curves.
  #endif
  
  /*
  //Working on a proper catmull-rom spline
  float catmullrom(float t, float p0, float p1, float p2, float p3)
  {
    0.5 * ( (2 * p1) + (-p0 + p2) * x + (2 * p0 - 5 * p1 + 4 * p2 - p3) * t * t + (-p0 + 3 * p1 - 3 * p2 + p3) * x * x * x );
  }
  
  //0.5 *(  (2 * b) + (-a + c) * x + (2*a - 5*b + 4*c - d) * x*x +(-a + 3*b- 3*c + d) * x*x*x)
  
  //y = 1/2 *(x (-a *(x-1)^2+c *(-3 * x^2+4 * x+1)+d * x *(x-1))+b (3 * x^3-5 * x^2+2)) //reduce
  //a * x * (x * (1 - 0.5 * x) - 0.5) + b * ((1.5 * x - 2.5) * x*x + 1) + x * (x * (x * (0.5 * d - 1.5 * c) + 2 * c - 0.5 * d) + 0.5 * c) //horner
  
  */

 	// -- Curve 6 --
  #if Curves_formula == 6
    x = x*x*x*(x*(x*6.0 - 15.0) + 10.0); //Perlins smootherstep - 752 amd fps , +vign 526 nv
	#endif
	
	// -- Curve 7 --
  #if Curves_formula == 7
    x = ((x-0.5) / ((0.5/(4.0/3.0)) + abs((x-0.5)*1.25))) + 0.5; // amd fps , +vign 527 nv
  #endif
	
  // -- Curve 8 --
  #if Curves_formula == 8
    x =  x * x *(2 * x-3)  *  (2 * x * x *x - 3 * x * x +3)  *  (4 * x * x * x -6 * x * x - 3) * 0.10; //Techicolor Cinestyle - almost identical to curve 1
  #endif
  
  // -- Curve 9 --
  #if Curves_formula == 9
    x = 0.5 -(abs(x*2-1)-2) * (x*2-1) * 0.5; //parabola
  #endif
  
  // -- Curve 10 --
  #if Curves_formula == 10 //Cubic catmull (say that 10 times fast)
    float a = 1.00; //control point 1
    float b = 0.00; //start point
    float c = 1.00; //endpoint
    float d = 0.20; //control point 2
    x = 0.5 * ((-a + 3*b -3*c + d)*x*x*x + (2*a -5*b + 4*c - d)*x*x + (-a+c)*x + 2*b); //A customizable cubic catmull-rom spline
  #endif

  // -- Curve 11 --
  #if Curves_formula == 11 //Cubic Bezier spline
    float a = 0.00; //start point
    float b = 0.00; //control point 1
    float c = 1.00; //control point 2
    float d = 1.00; //endpoint

    float r  = (1-x);
	float r2 = r*r;
	float r3 = r2 * r;
	float x2 = x*x;
	float x3 = x2*x;
	//x = dot(float4(a,b,c,d),float4(r3,3*r2*x,3*r*x2,x3)); 
	
	//x = a * r*r*r + r * (3 * b * r * x + 3 * c * x*x) + d * x*x*x;
	//x = a*(1-x)*(1-x)*(1-x) +(1-x) * (3*b * (1-x) * x + 3 * c * x*x) + d * x*x*x;
	x = a*(1-x)*(1-x)*(1-x) + 3*b*(1-x)*(1-x)*x + 3*c*(1-x)*x*x + d*x*x*x;
  #endif

  // -- Curve 12 --
  #if Curves_formula == 12 //Cubic Bezier spline - alternative implementation.
    float3 a = float3(0.00,0.00,0.00); //start point
    float3 b = float3(0.25,0.15,0.85); //control point 1
    float3 c = float3(0.75,0.85,0.15); //control point 2
    float3 d = float3(1.00,1.00,1.00); //endpoint

    float3 ab = lerp(a,b,x);           // point between a and b (green)
    float3 bc = lerp(b,c,x);           // point between b and c (green)
    float3 cd = lerp(c,d,x);           // point between c and d (green)
    float3 abbc = lerp(ab,bc,x);       // point between ab and bc (blue)
    float3 bccd = lerp(bc,cd,x);       // point between bc and cd (blue)
    float3 dest = lerp(abbc,bccd,x);   // point on the bezier-curve (black)
    x = dest;
  #endif
  
   /*-----------------------------------------------------------.   
  /                 Joining of Luma and Chroma                  /
  '-----------------------------------------------------------*/
	
  #if Curves_mode == 2 //Both Luma and Chroma
	float3 color = x;  //if the curve should be applied to both Luma and Chroma
	colorInput.rgb = lerp(colorInput.rgb, color, Curves_contrast_blend); //Blend by Curves_contrast
	
  #elif Curves_mode == 1 //Only Chroma
	x = x * 2 - 1; //adjust the Chroma range back to -1 -> 1
	float3 color = luma + x; //Luma + Chroma
	colorInput.rgb = lerp(colorInput.rgb, color, Curves_contrast_blend); //Blend by Curves_contrast
	
  #else // Curves_mode == 0 //Only Luma
    x = lerp(luma, x, Curves_contrast_blend); //Blend by Curves_contrast
    colorInput.rgb = x + chroma; //Luma + Chroma
    
  #endif

  //Return the result
  return colorInput;
}