#include "mfl_tier2.h"
/*00771a90*/ extern float g_data_771a90;
/*00771a94*/ extern float g_data_771a94;
/*00771aa4*/ extern undefined4 g_data_771aa4;
/* Resets the D3D9 pixel shader to g_data_771aa4 (inferred: the default shader handle) and zeroes the two float globals g_data_771a90 and g_data_771a94 (inferred: per-frame state; their ... */
/* Resets the D3D9 pixel shader to g_data_771aa4 (inferred: the default shader handle)
 * and zeroes the two float globals g_data_771a90 and g_data_771a94 (inferred: per-frame
 * state; their meaning is not known from this function alone).
 * Takes no parameters and returns nothing.
 */
void FUN_004aa220(void)
{
  rwD3D9SetPixelShader(g_data_771aa4);
  g_data_771a90 = 0;
  g_data_771a94 = 0;
  return;
}
