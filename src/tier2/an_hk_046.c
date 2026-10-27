#include "mfl_tier2.h"
/*0048ebf0*/ extern int rwp_car_collision_response(int p0, undefined4 p1, undefined4 p2);
/*0048f4a0*/ extern int rwp_collision_update(int p0, undefined4 p1, undefined4 p2);
/*0056e7c0*/ extern void FUN_0056e7c0(int p0, undefined4 p1);
/*00571850*/ extern void FUN_00571850(int p0);
/*005718a0*/ extern void FUN_005718a0(int p0);
/*00571970*/ extern void FUN_00571970(int p0);
/*00571990*/ extern void FUN_00571990(int p0);
/*00572a40*/ extern void FUN_00572a40(int p0);
/*00572d90*/ extern void FUN_00572d90(undefined4 p0, undefined4 p1, undefined4 p2, undefined4 p3, undefined4 p4, undefined4 p5, undefined4 p6, undefined4 p7, undefined4 p8, undefined4 p9, undefined4 p10, undefined4 p11, undefined4 p12, undefined4 p13, undefined4 p14, undefined4 p15);
/*00573650*/ extern void FUN_00573650(undefined4 p0);
/*00573860*/ extern void FUN_00573860(int p0);
/*00573880*/ extern void FUN_00573880(int p0);


/* rwp_world_step_pipeline */

/* "runs the fixed sequence of the physics step functions on the world" */

/* One physics step over the world: runs the step functions in a fixed order.
 * world: the world object, passed to every step function (inferred).
 * step_arg: forwarded to FUN_0056e7c0 and FUN_00572d90 (inferred: a step parameter, meaning unknown).
 * Returns nothing.
 * Locals world_for_<fn> are copies of world (the decompiler split them), one per call.
 * world_8c / world_90 are the fields at offsets 0x8c and 0x90 (meaning unknown);
 * the _reloaded copies are read again after the collision response, which may have changed them. */
void __cdecl rwp_world_step_pipeline(int world,undefined4 step_arg)

{
  int world_for_collision_update;
  undefined4 world_8c_reloaded;
  undefined4 world_90_reloaded;
  int world_for_573880;
  int world_for_572a40;
  int world_for_573860;
  int world_for_car_collision;
  undefined4 world_8c;
  undefined4 world_90;
  int world_for_571970;
  int world_for_571990;
  int world_for_5718a0;
  int world_for_571850;
  int world_for_572d90;

  /* Stage 1: pre-collision steps (inferred). */
  world_for_572d90 = world;
  FUN_0056e7c0(world,step_arg);
  world_for_571850 = world;
  FUN_00571850(world_for_571850);
  world_for_5718a0 = world;
  FUN_005718a0(world_for_5718a0);
  world_for_571990 = world;
  FUN_00571990(world_for_571990);
  world_for_571970 = world;
  FUN_00571970(world_for_571970);
  /* Car collision response, fed the fields at 0x8c and 0x90 (inferred). */
  world_90 = *(undefined4 *)(world + 0x90);
  world_8c = *(undefined4 *)(world + 0x8c);
  world_for_car_collision = world;
  rwp_car_collision_response(world_for_car_collision,world_8c,world_90);
  /* Stage 2: post-collision steps (inferred). */
  world_for_573860 = world;
  FUN_00573860(world_for_573860);
  world_for_572a40 = world;
  FUN_00572a40(world_for_572a40);
  world_for_573880 = world;
  FUN_00573880(world_for_573880);
  /* Collision update, using the fields re-read from 0x8c and 0x90 (inferred). */
  world_90_reloaded = *(undefined4 *)(world + 0x90);
  world_8c_reloaded = *(undefined4 *)(world + 0x8c);
  world_for_collision_update = world;
  rwp_collision_update(world_for_collision_update,world_8c_reloaded,world_90_reloaded);
  /* Final step batch, given the world, the step argument and every alias (inferred). */
  FUN_00572d90(world,world_for_collision_update,world_8c_reloaded,world_90_reloaded,world_for_573880,world_for_572a40,world_for_573860,world_for_car_collision,world_8c,world_90,world_for_571970,world_for_571990,world_for_5718a0,
               world_for_571850,world_for_572d90,step_arg);
  FUN_00573650(world);
  return;
}
