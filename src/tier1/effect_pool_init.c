#include "mfl_tier2.h"
/*0040d0d0*/ extern int __cdecl find_texture_dict_b(RwChar *p0);
/*00487bc0*/ extern int effect_pool_clear();
/*006c5e18*/ extern undefined4 g_gfx_6c5e18;


/* effect_pool_init */

/* "finds the effect texture and clears the pool" */

void effect_pool_init(void)
{
  g_gfx_6c5e18 = find_texture_dict_b("tyre");
  effect_pool_clear();
  return;
}
