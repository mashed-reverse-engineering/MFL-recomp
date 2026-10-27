#include "mfl_tier2.h"
/*00490400*/ extern undefined __cdecl rwp_world_step_pipeline(undefined4 p0, undefined4 p1);


/* rwp_world_step_forward */

/* "calls rwp_world_step_pipeline" */

undefined4 __cdecl rwp_world_step_forward(undefined4 world,undefined4 step)
{
  rwp_world_step_pipeline(world,step);
  return world;
}
