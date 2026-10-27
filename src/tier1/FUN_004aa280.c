#include "mfl_tier2.h"
/*00771a90*/ extern float g_data_771a90;
/*00771a94*/ extern float g_data_771a94;
/*00771aa8*/ extern undefined4 g_data_771aa8;
/* FUN_004aa280
 * Binds the pixel shader in g_data_771aa8 (rwD3D9SetPixelShader), then copies the
 * two 32-bit words at src_pair into the float globals g_data_771a90 and g_data_771a94.
 * src_pair: pointer to two 4-byte values, read as [0] -> g_data_771a90 and [1] -> g_data_771a94
 * (inferred: a 2-float pair, e.g. x/y; meaning of the globals not known).
 * Returns nothing.
 */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
void FUN_004aa280(undefined4 *src_pair)
{
  rwD3D9SetPixelShader(g_data_771aa8);
  *(undefined4 *)&g_data_771a90 = *src_pair;
  *(undefined4 *)&g_data_771a94 = src_pair[1];
  return;
}
