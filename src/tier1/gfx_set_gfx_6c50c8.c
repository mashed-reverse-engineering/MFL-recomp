#include "mfl_tier2.h"
/*006c50c8*/ extern float g_gfx_6c50c8;
/* gfx_set_gfx_6c50c8 */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* "g_gfx_6c50c8 = param_1; return;" */
/* Stores the raw 32-bit pattern value_bits into the float global g_gfx_6c50c8
   (bit copy, no float conversion). Callers' meaning of the value is inferred:
   a graphics setting. Returns nothing. */
void gfx_set_gfx_6c50c8(undefined4 value_bits)
{
  *(int *)&g_gfx_6c50c8 = value_bits;
  return;
}
