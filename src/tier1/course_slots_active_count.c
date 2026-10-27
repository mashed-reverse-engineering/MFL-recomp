#include "mfl_tier2.h"
/*0042a160*/ extern int course_active_objects_count(void);
int course_slots_active_count(void)
{
  return course_active_objects_count();
}
