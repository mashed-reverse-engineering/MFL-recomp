#include "mfl_tier2.h"
/*0040fdc0*/ extern int race_get_phase();
/*00468b80*/ extern int voice_queue_remove();


/* voice_queue_clear_players_when_phase_2 */

/* "removes the voice lines of the four players from both queues when the race phase is 2" */

void voice_queue_clear_players_when_phase_2(void)
{
  int race_phase;
  
  race_phase = race_get_phase();
  if (race_phase == 2) {
    race_phase = 0;
    do {
      voice_queue_remove(1,race_phase,0xffffffff);
      voice_queue_remove(0,race_phase,0xffffffff);
      race_phase = race_phase + 1;
    } while (race_phase < 4);
  }
  return;
}
