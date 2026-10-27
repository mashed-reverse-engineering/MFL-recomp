#include "mfl_tier2.h"
/*00771a90*/ extern float g_data_771a90;
/*00771a94*/ extern float g_data_771a94;
/*00771aac*/ extern undefined4 g_data_771aac;
/* FUN_004aa250 */
/* Sets the pixel shader g_data_771aac, then copies the first two 32-bit words of
   src_values into the float globals g_data_771a90 and g_data_771a94 (inferred: a
   pair of floats such as an offset or scale). Takes no return value. */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
void FUN_004aa250(undefined4 *src_values)
{
  rwD3D9SetPixelShader(g_data_771aac);
  *(undefined4 *)&g_data_771a90 = *src_values;
  *(undefined4 *)&g_data_771a94 = src_values[1];
  return;
}
