#include "mfl_tier2.h"
/*00434bb0*/ extern int get_event_type();
/*00459810*/ extern int course_object_offsets_init();
/*0060d92c*/ extern undefined4 g_course_highway_60d92c;
/*0060d930*/ extern undefined4 g_course_highway_60d930;
/*0060d934*/ extern undefined4 g_course_highway_60d934;
/*006a2684*/ extern float g_course_highway_6a2684;
/*006a2688*/ extern float g_course_highway_6a2688;
/*006a268c*/ extern float g_course_highway_6a268c;
/*006a26b4*/ extern undefined4 g_course_highway_6a26b4;
/* course_highway_cb11 */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* Course setup step for course_highway_cb11 (inferred from the name and the calls).
 * Takes no parameters and returns nothing.
 * Clears g_course_highway_6a26b4, then calls course_object_offsets_init() when
 * get_event_type() is 0, 1 or 2 (inferred: event kinds).
 * If the flag is nonzero afterwards, copies the words in g_course_highway_60d92c/30/34
 * into the float globals 6a2684/88/8c as raw bits (int copy, not a float conversion).
 * Who sets the flag is not visible here (inferred: course_object_offsets_init).
 */
void course_highway_cb11(void)
{
  int event_type;
  
  g_course_highway_6a26b4 = 0;
  /* get_event_type() is called again for each comparison (up to 3 calls), not cached */
  event_type = get_event_type();
  if (event_type != EVENT_TIME_TRIAL_0) {
    event_type = get_event_type();
    if (event_type != EVENT_TIME_TRIAL_1) {
      event_type = get_event_type();
      if (event_type != EVENT_TIME_TRIAL_2) goto LAB_0045a9cc;
    }
  }
  course_object_offsets_init();
/* event type not 0, 1 or 2: skip course_object_offsets_init() */
LAB_0045a9cc:
  /* copy the three 60d9xx words into the float globals as raw bits (inferred: course data) */
  if (g_course_highway_6a26b4 != 0) {
    *(int *)&g_course_highway_6a2684 = g_course_highway_60d92c;
    *(int *)&g_course_highway_6a2688 = g_course_highway_60d930;
    *(int *)&g_course_highway_6a268c = g_course_highway_60d934;
  }
  return;
}
