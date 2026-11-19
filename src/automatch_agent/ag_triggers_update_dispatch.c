#include "mfl_tier2.h"
/*00496940*/ extern int triggers_update();
/*00496ad0*/ extern int triggers_sort_axis();
/*0071aaf0*/ extern undefined4 g_trigger_71aaf0;
/*0071aaf4*/ extern undefined4 g_trigger_count;
/*0071aafc*/ extern undefined4 g_particles_71aafc;
/*0071ab00*/ extern undefined4 g_particles_71ab00;


/* triggers_update_dispatch */

/* "calls triggers_sort_axis or the other path depending on g_trigger_71aaf0" */

void triggers_update_dispatch(void)

{
  if (g_trigger_71aaf0 != 0) {
    triggers_update();
  }
  else {
    triggers_sort_axis();
  }
  g_particles_71ab00 = g_trigger_count;
  g_trigger_count = 0;
  g_particles_71aafc = 0;
  return;
}
