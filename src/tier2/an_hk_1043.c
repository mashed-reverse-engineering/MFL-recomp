#include "mfl_tier2.h"
/*004c52b0*/ extern int FUN_004c52b0(undefined4 p0, undefined4 p1, undefined4 p2);
/* load_shadow_models_cb_456540 */

/* function pointer pushed/stored by load_shadow_models (PUSH 0x456540) */

undefined4 load_shadow_models_cb_456540(undefined4 param_1,undefined4 *param_2)

{
  undefined4 uVar1;
  
  uVar1 = FUN_004c52b0(param_1,0,0);
  switch(uVar1) {
  case 1:
    *param_2 = param_1;
    param_2[4] = param_2[4] + 1;
    return param_1;
  case 2:
    param_2[1] = param_1;
    param_2[4] = param_2[4] + 1;
    return param_1;
  case 3:
    param_2[2] = param_1;
    param_2[4] = param_2[4] + 1;
    return param_1;
  case 4:
    param_2[3] = param_1;
  }
  param_2[4] = param_2[4] + 1;
  return param_1;
}


