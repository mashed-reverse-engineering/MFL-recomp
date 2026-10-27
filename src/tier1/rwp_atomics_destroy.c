#include "mfl_tier2.h"
/*004a5da0*/ extern int object_destroy_frames();
/*004f6f40*/ extern RwBool RpClumpDestroy(struct RpClump *p0);
/*006fa7d8*/ extern undefined4 g_rw_6fa7d8;
/*006fd418*/ extern undefined4 g_physics_6fd418;
/*00701c94*/ extern int g_rw_701c94;
/*00701c98*/ extern int g_physics_701c98;


/* rwp_atomics_destroy */

/* Destroys registered atomic frames and physics clumps, resetting their counts to zero. */

void rwp_atomics_destroy(void)
{
  int index;
  
  /* Destroy frames for registered RenderWare atomics */
  index = 0;
  if (0 < g_rw_701c94) {
    do {
      object_destroy_frames((&g_rw_6fa7d8)[index]);
      index = index + 1;
    } while (index < g_rw_701c94);
  }
  /* Destroy registered physics clumps */
  index = 0;
  if (0 < g_physics_701c98) {
    do {
      RpClumpDestroy((RpClump *)(&g_physics_6fd418)[index]);
      index = index + 1;
    } while (index < g_physics_701c98);
  }
  /* Reset counts */
  g_physics_701c98 = 0;
  g_rw_701c94 = 0;
  return;
}
