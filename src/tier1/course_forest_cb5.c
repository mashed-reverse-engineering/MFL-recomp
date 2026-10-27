#include "mfl_tier2.h"
/*00458f80*/ extern int SUB_00458f80();
/*00484ad0*/ extern int pool_list_destroy();
/*008c4b08*/ extern undefined4 *g_course_forest_8c4b08;
/* course_forest_cb5 */
/* destroys the forest pool list; the original ends in a tail jump to pool_list_destroy */
int course_forest_cb5(int course)
{
  SUB_00458f80(course);
  return pool_list_destroy(g_course_forest_8c4b08);
}
