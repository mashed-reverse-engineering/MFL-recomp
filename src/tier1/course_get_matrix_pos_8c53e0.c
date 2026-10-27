#include "mfl_tier2.h"
/*008c53e0*/ extern char *g_course_8c53e0;


/* course_get_matrix_pos_8c53e0 */

/* "frame position (+0x40) of the object at g_course_8c53e0" */

int course_get_matrix_pos_8c53e0(void)
{
  return *(int *)(g_course_8c53e0 + 4) + 0x40;
}
