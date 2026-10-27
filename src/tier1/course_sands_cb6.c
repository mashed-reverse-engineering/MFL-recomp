#include "mfl_tier2.h"
/*0040fdc0*/ extern int race_get_phase();
/*0045b510*/ extern int course_marker_positions_init();


/* course_sands_cb6 */

void course_sands_cb6(void)
{
  int race_phase;
  
  race_phase = race_get_phase();
  if (race_phase == 1) {
    course_marker_positions_init();
    return;
  }
  race_get_phase();
  return;
}
