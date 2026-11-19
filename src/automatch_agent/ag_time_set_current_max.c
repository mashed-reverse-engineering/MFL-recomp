#include "mfl_tier2.h"
/*00413ad0*/ extern int replay_best_get_split_time();
/*004364c0*/ extern int ui_set_triple();
/*0069bb5c*/ extern undefined4 g_course_69bb5c;
/*0069bb64*/ extern undefined4 g_course_69bb64;
/*0069bb6c*/ extern float g_course_69bb6c;
/*008c879c*/ extern undefined4 g_course_8c879c;


/* time_set_current_max */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* "sets the current time to 9:59.99 (0x3F7D70A4 = 0.99 fraction)" */

void time_set_current_max(void)

{
  undefined4 minutes;
  undefined4 seconds;
  undefined4 fraction_bits;
  
  g_course_69bb6c = 0.99f;
  g_course_69bb64 = 0x3b;
  g_course_69bb5c = 9;
  g_course_8c879c = 0;
  fraction_bits = 0x3f7d70a4;
  seconds = 0x3b;
  minutes = 9;
  ui_set_triple(0,0,0x3f7d70a4,0x3b,9);
  replay_best_get_split_time(0,&fraction_bits,&seconds,&minutes);
  ui_set_triple(0,1,fraction_bits,seconds,minutes);
  fraction_bits = 0x3f7d70a4;
  seconds = 0x3b;
  minutes = 9;
  ui_set_triple(1,0,0x3f7d70a4,0x3b,9);
  replay_best_get_split_time(1,&fraction_bits,&seconds,&minutes);
  ui_set_triple(1,1,fraction_bits,seconds,minutes);
  return;
}
