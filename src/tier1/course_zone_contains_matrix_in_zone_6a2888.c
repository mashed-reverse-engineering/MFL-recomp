#include "mfl_tier2.h"
/*006a2888*/ extern undefined g_course_6a2888;


/* course_zone_contains_matrix_in_zone_6a2888 */

/* "course_zone_contains_matrix on the zone record at g_course_6a2888" */

/* Returns the zone test result; rocket_surface_hit_test 0x4683c9 tests it (VOID_w16: was void). */
int course_zone_contains_matrix_in_zone_6a2888(undefined4 param_1,undefined4 param_2)
{
  return course_zone_contains_matrix(&g_course_6a2888,param_1,param_2);
}
