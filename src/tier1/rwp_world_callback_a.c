#include "mfl_tier2.h"
/*0056a710*/ extern void FUN_0056a710(undefined4 p0, undefined4 p1);
/*0056e8c0*/ extern int FUN_0056e8c0(undefined4 p0);
/* rwp_world_callback_a */
/* "physics world callback: FUN_0056a710 + FUN_0056e8c0 (per object)" */
/* Physics world callback, run once per object. Calls FUN_0056a710(world, object)
 * then FUN_0056e8c0(object). Parameters: object = the object handle (inferred),
 * world = the physics world it belongs to (inferred from the callback name).
 * Returns the object handle unchanged. */
undefined4 rwp_world_callback_a(undefined4 object,undefined4 world)
{
  FUN_0056a710(world,object);
  FUN_0056e8c0(object);
  return object;
}
