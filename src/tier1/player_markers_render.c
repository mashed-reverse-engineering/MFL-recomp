#include "mfl_tier2.h"
/*004f67c0*/ extern struct RpClump * RpClumpRender(struct RpClump *p0);
/*006c3f3c*/ extern undefined4 g_gfx_6c3f3c;
/*006c3fdc*/ extern char g_car_6c3fdc[];


/* player_markers_render */

/* "renders the clumps of the active markers" */

void player_markers_render(void)
{
  undefined4 *puVar1;
  
  puVar1 = &g_gfx_6c3f3c;
  do {
    if ((puVar1[-1] != 0) && ((RpClump *)*puVar1 != (RpClump *)0x0)) {
      RpClumpRender((RpClump *)*puVar1);
    }
    puVar1 = puVar1 + 4;
  } while ((int)puVar1 < ((int)&g_car_6c3fdc));
  return;
}
