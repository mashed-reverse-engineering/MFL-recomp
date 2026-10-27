#include "mfl_tier2.h"
/*00429620*/ extern int race_setup_struct_1();
/*008d8b14*/ extern undefined4 g_race_setup;


/* race_setup_use_1 */

/*  */

void race_setup_use_1(void)
{
  g_race_setup = race_setup_struct_1();
  return;
}
