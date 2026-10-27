#include "mfl_tier2.h"
/*00771ab0*/ extern char *g_data_771ab0;
/*007e06e0*/ extern undefined4 g_data_7e06e0;
/* course_storm_cb2_sub_4ab140 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* single caller course_storm_cb2 */

void course_storm_cb2_sub_4ab140(void)

{
  if (g_data_7e06e0 != 0) {
    (**(code **)(g_data_771ab0 + 0x48))(g_data_771ab0);
  }
  return;
}


