#include "mfl_tier2.h"
/*0048ebf0*/ extern void __cdecl rwp_car_collision_response(int world, int a, int b);
/*0048f4a0*/ extern void __cdecl rwp_collision_update(int world, int a, int b);
/*0056e7c0*/ extern void __cdecl FUN_0056e7c0(int world, int arg);
/*00571850*/ extern void __cdecl FUN_00571850(int world);
/*005718a0*/ extern void __cdecl FUN_005718a0(int world);
/*00571970*/ extern void __cdecl FUN_00571970(int world);
/*00571990*/ extern void __cdecl FUN_00571990(int world);
/*00572a40*/ extern void __cdecl FUN_00572a40(int world);
/*00572d90*/ extern void __cdecl FUN_00572d90(int world);
/*00573650*/ extern void __cdecl FUN_00573650(int world);
/*00573860*/ extern void __cdecl FUN_00573860(int world);
/*00573880*/ extern void __cdecl FUN_00573880(int world);
#define W(w, off) (*(int *)((char *)(w) + (off)))
/* rwp_world_step_pipeline */

/* One physics step of the world: runs the step functions in a fixed order around the car collision response and the collision update. */
void __cdecl rwp_world_step_pipeline(int world, int step_arg)
{
  FUN_0056e7c0(world, step_arg);
  FUN_00571850(world);
  FUN_005718a0(world);
  FUN_00571990(world);
  FUN_00571970(world);
  rwp_car_collision_response(world, W(world, 0x8c), W(world, 0x90));
  FUN_00573860(world);
  FUN_00572a40(world);
  FUN_00573880(world);
  rwp_collision_update(world, W(world, 0x8c), W(world, 0x90));
  FUN_00572d90(world);
  FUN_00573650(world);
}
