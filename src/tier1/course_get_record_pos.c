#include "mfl_tier2.h"
extern char g_course_lap_data_field49612_0xc1cc[];

#define COURSE_RECORD_STRIDE 0x4C     /* bytes per course record */
#define COURSE_RECORD_POS_TABLE ((int)&g_course_lap_data_field49612_0xc1cc) /* base address of the position records */


/* course_get_record_pos */

/* "(i): 0x681834 + i * 0x4C" */

/* Address of position record number index. */
int course_get_record_pos(int index)
{
  return index * COURSE_RECORD_STRIDE + COURSE_RECORD_POS_TABLE;
}
