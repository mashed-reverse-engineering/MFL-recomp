#include "mfl_tier2.h"
/*00652f58*/ extern undefined4 g_race_652f58;


/* race_set_setup_index */

/* Sets the index of the current race in the race setup. */
/* "0x652F58 = index of the current race in the race setup" */

void race_set_setup_index(undefined4 setup_index)
{
  g_race_652f58 = setup_index;
  return;
}
