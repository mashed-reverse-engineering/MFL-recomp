// cluster-entries: rwp_world_register_callbacks_cb_494320
/* Installed cluster rwp_world_register_callbacks_cb_494320 (tools/cluster_install.py from output/cluster_pilot_kd/rwp_world_register_callbacks_cb_494320.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*0056a4b0*/ extern void FUN_0056a4b0(undefined4 p0, undefined4 p1);
/*0058ca00*/ extern void FUN_0058ca00(undefined4 p0);
/*005b5fa0*/ extern int __cdecl FUN_005b5fa0(int a1);
/* rwp_world_register_callbacks_cb_494320 */

/* function pointer pushed/stored by rwp_world_register_callbacks (PUSH 0x494320) */

undefined4 * rwp_world_register_callbacks_cb_494320(undefined4 *param_1, undefined4 param_2)

{
  FUN_0056a4b0(param_2,param_1);
  if (*(short *)*param_1 == 0) {
    FUN_0058ca00(param_1);
  }
  else if (*(short *)*param_1 == 1) {
    FUN_005b5fa0(param_1);
    return param_1;
  }
  return param_1;
}

