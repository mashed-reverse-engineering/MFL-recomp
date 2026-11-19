#include "mfl_tier2.h"
/*00495a20*/ extern int __cdecl rwp_raster_pair_create(undefined4 *p0, RwInt32 p1, RwInt32 p2, uint p3);
/*00702244*/ extern undefined g_physics_702244;
/*00702254*/ extern undefined g_physics_702254;


/* viewport_raster_create */

/* "(index 0..3, w, h, flags): creates the raster pair of a viewport" */

int viewport_raster_create(uint viewport_index,RwInt32 width,RwInt32 height,uint flags)

{
  int *raster_slot;
  int raster_pair;

  if (viewport_index >= 4) {
    return 0;
  }
  if (*(int *)(&g_physics_702244 + viewport_index * 4) != 0) {
    return 0;
  }
  raster_slot = (int *)(&g_physics_702254 + viewport_index * 4);
  if (*raster_slot != 0) {
    return 0;
  }
  raster_pair = rwp_raster_pair_create(raster_slot,width,height,flags);
  *(int *)(&g_physics_702244 + viewport_index * 4) = raster_pair;
  if (raster_pair == 0) {
    return 0;
  }
  return *raster_slot != 0;
}
