#include "mfl_tier2.h"
/*00429660*/ extern int race_setup_struct_4();
/*008d8b14*/ extern undefined4 g_race_setup;


/* race_setup_use_4 */

/*  */

/* Makes race setup record 4 the current g_race_setup. */
void race_setup_use_4(void)
{
  g_race_setup = race_setup_struct_4();
  return;
}
