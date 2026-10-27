#include "mfl_tier2.h"
/*004815e0*/ extern undefined __cdecl atomic_bbox_calculate(struct RwBBox *p0, int p1);
/*0089de0c*/ extern char *g_gfx_89de0c;
/* atomic_plugin_register_cb_4819a0 */

/* function pointer pushed/stored by atomic_plugin_register (PUSH 0x4819a0) */

undefined4 atomic_plugin_register_cb_4819a0(int param_1)

{
  if (*(float *)(g_gfx_89de0c + 8 + param_1) == *(float *)(g_gfx_89de0c + 0x14 + param_1)) {
    atomic_bbox_calculate((RwBBox *)(g_gfx_89de0c + 8 + param_1),param_1);
  }
  return 1;
}


