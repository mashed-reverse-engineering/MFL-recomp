#include "mfl_tier2.h"
/*00486b00*/ extern int ptank_set_blend_modes();
/* Offset of the PTank sub-block inside the fx state (inferred) */
#define FX_STATE_PTANK_OFFSET (0x10)
/* fx_set_blend_modes */
/* "gfx_fn_486b00 (sets src / dest blend of the PTank)" */
/*
 * Sets the source / destination blend modes of the PTank effect.
 * fx_state: effect state; the PTank sub-block is at FX_STATE_PTANK_OFFSET.
 * arg2, arg3: forwarded unchanged to ptank_set_blend_modes (inferred: blend mode values).
 * Returns nothing.
 */
void fx_set_blend_modes(undefined4 fx_state, undefined4 arg2, undefined4 arg3)
{
  ptank_set_blend_modes((fx_state + FX_STATE_PTANK_OFFSET), arg2, arg3);
  return;
}
