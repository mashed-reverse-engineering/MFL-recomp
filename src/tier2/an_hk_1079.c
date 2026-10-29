#include "mfl_tier2.h"
/*00491180*/ extern int rwp_contact_collect();
/*00549290*/ extern int RpCollisionWorldForAllIntersections();
/* rwp_world_create_once_cb_491280 */

/* function pointer pushed/stored by rwp_world_create_once (PUSH 0x491280) */

/* TIER2: unvalidated 2026-10-27 authority=diffemu */
/* Collision callback: copies the 6-float box from `box` into a query frame and runs RpCollisionWorldForAllIntersections on the world, collecting contacts into collect_state; returns world (inferred). */
/* TIER2: unvalidated 2026-10-29 authority=diffemu */
int rwp_world_create_once_cb_491280(int world,char *unused_2,undefined4 *box,char *unused_4,char *collect_state)
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
  
  *(undefined4 *)(collect_state + 4) = 0;
  *(undefined4 *)(collect_state + 0xc) = 0;
  frame.uStack_10 = box[4];
  frame.uStack_c = box[5];
  frame.uStack_8 = box[6];
  frame.uStack_1c = *box;
  frame.uStack_18 = box[1];
  frame.uStack_14 = box[2];
  frame.uStack_4 = 4;
  RpCollisionWorldForAllIntersections
            (*(undefined4 *)(world + 0x40),&frame.uStack_1c,rwp_contact_collect,collect_state);
  return world;
}


