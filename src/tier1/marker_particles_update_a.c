#include "mfl_tier2.h"
/*0040fdc0*/ extern int race_get_phase();
/*0041d200*/ extern int hudpanel_markers_update_active();
#define RACE_PHASE_MARKERS (6) /* (inferred) race phase in which markers are drawn */
/* marker_particles_update_a */
/* "race phase 6: hudpanel_markers_update_active when menu_markers_enabled_for_event() (markers enabled)" */
/* Per-frame marker update: during RACE_PHASE_MARKERS, if markers are enabled
   (menu_markers_enabled_for_event), refresh the active HUD marker panel.
   Takes no parameters; returns nothing. The local is reused: first the race
   phase, then the markers-enabled flag. */
void marker_particles_update_a(void)
{
  int race_phase;

  race_phase = race_get_phase();
  if (race_phase == RACE_PHASE_MARKERS) {
    /* markers enabled? (flag returned in race_phase) */
    race_phase = menu_markers_enabled_for_event();
    if (race_phase != 0) {
      hudpanel_markers_update_active();
      return;
    }
  }
  return;
}
