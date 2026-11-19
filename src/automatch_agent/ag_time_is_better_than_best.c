#include "mfl_tier2.h"
/*0069bb5c*/ extern int g_course_69bb5c;
/*0069bb60*/ extern int g_best_69bb60;
/*0069bb64*/ extern int g_course_69bb64;
/*0069bb68*/ extern int g_best_69bb68;
/*0069bb6c*/ extern float g_course_69bb6c;
/*0069bb70*/ extern float g_best_69bb70;


/* time_is_better_than_best */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* "compares the current time (min * 60 + sec + fraction) with the best time" */

undefined4 time_is_better_than_best(void)

{
  if ((float)(g_course_69bb5c * 0x3c + g_course_69bb64) + g_course_69bb6c <
      (float)(g_best_69bb60 * 0x3c + g_best_69bb68) + g_best_69bb70) {
    return 1;
  }
  return 0;
}
