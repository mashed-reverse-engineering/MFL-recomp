#include "mfl_tier2.h"
/*00429640*/ extern int race_setup_struct_2();
/*008d8b14*/ extern undefined4 g_race_setup;


/* race_setup_use_2 */

/*  */

/* Makes race setup record 2 the current g_race_setup. */
void race_setup_use_2(void)
{
  g_race_setup = race_setup_struct_2();
  return;
}
