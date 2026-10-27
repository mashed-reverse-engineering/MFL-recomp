#include "mfl_tier2.h"
/*004f67c0*/ extern int RpClumpRender();
/* course_egypt_cb2 */
/* tail call: the original rewrites its stack argument and jumps */
int course_egypt_cb2(char *course)
{
  return RpClumpRender(((CourseWorld *)course)->clumps[0]);
}
