#include "mfl_tier2.h"
/*006a06e0*/ extern char *g_sky_atomic;


/* course_sky_atomic_render */

/* "calls the atomic render callback of g_course_6a06e0 if set" */

void course_sky_atomic_render(void)
{
  if (g_sky_atomic != 0) {
    (**(code **)(g_sky_atomic + 0x48))(g_sky_atomic);
  }
  return;
}
