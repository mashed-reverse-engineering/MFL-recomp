#include "mfl_tier2.h"
/*00457170*/ extern int spline_object_update();
/*004572c0*/ extern int spline_objects_resolve_pair(void *param_2, void *param_3);
/*005dda20*/ extern float k_f_4_2949673e9;
/*005de490*/ extern float k_f_0_02;
/*008c5500*/ extern undefined g_course_spline_8c5500[0x1000];
/*008c5608*/ extern char g_course_spline_8c5608[];
#define SPLINE_OBJECT_STRIDE (0x58)
#define SPLINE_OBJECT_COUNT (3)
/* course_spline_objects_update */

/* "(ticks): updates the 3 spline objects and resolves their pairs" */

/* Advances the course spline objects by one update.
 * ticks: game tick count (inferred).
 * Converts ticks to time_seconds (inferred: 50 ticks per second, times 0.02f).
 * Negative ticks are read as unsigned 32-bit (2^32 is added before scaling).
 * Resolves the pairs, then updates each object with the float bits of time_seconds.
 * Returns nothing. */

void course_spline_objects_update(int ticks)

{
  int next_index;
  float time_seconds;
  int object_index;
  undefined *pair_object;
  undefined *spline_object;

  time_seconds = (float)ticks;
  if (ticks < 0) {
    time_seconds = time_seconds + k_f_4_2949673e9;
  }
  time_seconds = time_seconds * k_f_0_02;
  spline_object = g_course_spline_8c5500;
  object_index = 0;
  /* Pair each object with every later object: (0,1), (0,2), (1,2).
     The pair loop only runs while next_index < SPLINE_OBJECT_COUNT.
     The return value of spline_objects_resolve_pair is overwritten below and never read. */
  do {
    next_index = object_index + 1;
    if (next_index < SPLINE_OBJECT_COUNT) {
      pair_object = spline_object + SPLINE_OBJECT_STRIDE;
      do {
        object_index = spline_objects_resolve_pair(spline_object,pair_object);
        pair_object = pair_object + SPLINE_OBJECT_STRIDE;
      } while ((char *)pair_object < ((char *)&g_course_spline_8c5608));
    }
    spline_object = spline_object + SPLINE_OBJECT_STRIDE;
    object_index = next_index;
  } while ((char *)spline_object < ((char *)&g_course_spline_8c5608));
  spline_object = g_course_spline_8c5500;
  /* Update every spline object with the time (float bits passed as int). */
  do {
    spline_object_update(spline_object,*(int *)&time_seconds);
    spline_object = spline_object + SPLINE_OBJECT_STRIDE;
  } while ((char *)spline_object < ((char *)&g_course_spline_8c5608));
  return;
}


