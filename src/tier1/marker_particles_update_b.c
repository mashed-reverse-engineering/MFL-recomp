#include "mfl_tier2.h"
/*0040fdc0*/ extern int race_get_phase();
/*0041c510*/ extern int hudpanel_markers_render_all();
/*004353e0*/ extern int menu_markers_enabled_for_event();


/* marker_particles_update_b */

/* "race phase 6: FUN_0041c510 when markers enabled" */

void marker_particles_update_b(void)
{
  int race_phase;
  
  race_phase = race_get_phase();
  if (race_phase == 6) {
    race_phase = menu_markers_enabled_for_event();
    if (race_phase != 0) {
      hudpanel_markers_render_all();
      return;
    }
  }
  return;
}
