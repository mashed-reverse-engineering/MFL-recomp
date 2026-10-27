#include "mfl_tier2.h"
/*00491180*/ extern int rwp_contact_collect();
/*00549290*/ extern int RpCollisionWorldForAllIntersections();
/* rwp_world_create_once_cb_491280 */

/* function pointer pushed/stored by rwp_world_create_once (PUSH 0x491280) */

/* TIER2: unvalidated 2026-10-27 authority=static */
int rwp_world_create_once_cb_491280(int param_1,char *unused_2,undefined4 *param_3,char *unused_4,char *param_5)
{
  struct {
    undefined4 uStack_1c;
    undefined4 uStack_18;
    undefined4 uStack_14;
    undefined4 uStack_10;
    undefined4 uStack_c;
    undefined4 uStack_8;
    undefined4 uStack_4;
  } frame;
  
  *(undefined4 *)(param_5 + 4) = 0;
  *(undefined4 *)(param_5 + 0xc) = 0;
  frame.uStack_10 = param_3[4];
  frame.uStack_c = param_3[5];
  frame.uStack_8 = param_3[6];
  frame.uStack_1c = *param_3;
  frame.uStack_18 = param_3[1];
  frame.uStack_14 = param_3[2];
  frame.uStack_4 = 4;
  RpCollisionWorldForAllIntersections
            (*(undefined4 *)(param_1 + 0x40),&frame.uStack_1c,rwp_contact_collect,param_5);
  return param_1;
}


