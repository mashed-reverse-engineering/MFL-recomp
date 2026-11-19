#include "mfl_tier2.h"
extern undefined g_course_world;
/*00421a40*/ extern int course_call_cb10();
/*00421aa0*/ extern int course_select();
/*00421b60*/ extern int course_has_cb10();
/*00675668*/ extern struct CourseLap g_course_lap_data;


/* course_hook10 */

/* "cb10 when the course is loaded (split_sectors[8] != 0)" */

void course_hook10(undefined4 param_1)

{
  int iVar1;
  
  if (g_course_lap_data.split_sectors[8] != 0) {
    course_select(param_1);
    iVar1 = course_has_cb10();
    if (iVar1 != 0) {
      course_call_cb10((char *)&g_course_world);
      return;
    }
  }
  return;
}
