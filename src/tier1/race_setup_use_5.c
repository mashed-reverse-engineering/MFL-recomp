#include "mfl_tier2.h"
/*00429670*/ extern int race_setup_struct_5();
/*005dd330*/ extern float float_1_0;
/*007ef758*/ extern float g_race_setup_7ef758;
/*008d8b14*/ extern undefined4 g_race_setup;


/* race_setup_use_5 */

/* "also advances the float at 0x7EF758 by 1.0" */

void race_setup_use_5(void)
{
  g_race_setup_7ef758 = g_race_setup_7ef758 + float_1_0;
  g_race_setup = race_setup_struct_5();
  return;
}
