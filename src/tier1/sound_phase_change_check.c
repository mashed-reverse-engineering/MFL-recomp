#include "mfl_tier2.h"
/*0040fdc0*/ extern int race_get_phase();
/*0040ff00*/ extern int race_get_race_652dec();
/*008bed00*/ extern undefined4 g_sound_frame_8bed00;


/* sound_phase_change_check */

/* "calls race_get_race_652dec when the race phase differs from the stored one" */

void sound_phase_change_check(void)
{
  int race_phase;
  
  race_phase = race_get_phase();
  if (race_phase != g_sound_frame_8bed00) {
    race_get_race_652dec();
    return;
  }
  return;
}
