#include "mfl_tier2.h"
/*004f67c0*/ extern struct RpClump * RpClumpRender(struct RpClump *p0);
/*006c3f3c*/ extern undefined4 g_gfx_6c3f3c;
/*006c3fdc*/ extern char g_car_6c3fdc[];


/* player_markers_render */

/* "renders the clumps of the active markers" */

void player_markers_render(void)
{
  undefined4 *marker_slot;
  
  marker_slot = &g_gfx_6c3f3c;
  do {
    if ((marker_slot[-1] != 0) && ((RpClump *)*marker_slot != (RpClump *)0x0)) {
      RpClumpRender((RpClump *)*marker_slot);
    }
    marker_slot = marker_slot + 4;
  } while ((int)marker_slot < ((int)&g_car_6c3fdc));
  return;
}
