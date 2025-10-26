/*
------------------------
16-235 -> 0-255 (Levels)
------------------------

- Expands the 16-235 TV range to 0-255 PC range
*/
#define const_1 (DARK_LEVEL/255.0)
#define const_2 (255.0/(255.0-BRIGHT_LEVEL))

float4 TVLevelsPass( float4 colorInput )
{
  colorInput.rgb = (colorInput.rgb  - const_1) * const_2;
  return colorInput;
}