#include "mfl_tier2.h"
/*00481620*/ extern undefined __cdecl atomic_render_with_culling(struct RpAtomic *p0);
/*0089de08*/ extern undefined4 g_gfx_89de08;

#define ATOMIC_RENDER_CB_OFFSET 0x48          /* render callback field in RpAtomic */
#define FALLBACK_RENDER_CB ((code *)0x4f60d0) /* used only if the culling callback were null */

/* atomic_set_render_callback */

/* WARNING: Removing unreachable block (ram,0x0048177d) */
/* Saves the atomic's current render callback to g_gfx_89de08 and installs atomic_render_with_culling. */

void __cdecl atomic_set_render_callback(char *atomic,undefined4 unused_arg)

{
  g_gfx_89de08 = *(undefined4 *)(atomic + ATOMIC_RENDER_CB_OFFSET);
  *(code **)(atomic + ATOMIC_RENDER_CB_OFFSET) = atomic_render_with_culling;
  if (atomic_render_with_culling != (code *)0x0) {
    return;
  }
  *(code **)(atomic + ATOMIC_RENDER_CB_OFFSET) = FALLBACK_RENDER_CB;
  return;
}
