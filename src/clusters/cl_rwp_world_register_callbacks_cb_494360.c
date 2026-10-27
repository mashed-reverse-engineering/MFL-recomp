// cluster-entries: rwp_world_register_callbacks_cb_494360
/* Installed cluster rwp_world_register_callbacks_cb_494360 (tools/cluster_install.py from output/cluster_pilot_md/rwp_world_register_callbacks_cb_494360.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*0056e440*/ extern void FUN_0056e440(param_2, param_1);
/*0056c060*/ extern void __cdecl FUN_0056c060(undefined4 p0, undefined4 p1, undefined4 p2);
/* rwp_world_register_callbacks_cb_494360 */

/* "function pointer pushed/stored by rwp_world_register_callbacks (PUSH 0x494360)" */
undefined4 rwp_world_register_callbacks_cb_494360(undefined4 param_1, undefined4 param_2)
{
  FUN_0056e440(param_2, param_1);
  FUN_0056c060(param_1, param_2, param_1);
  return param_1;
}

