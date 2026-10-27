#include "mfl_tier2.h"
extern char g_course_lap_data_field49600_0xc1c0[];

#define COURSE_RECORD_STRIDE 0x4C     /* bytes per course record */
#define COURSE_RECORD_BASE_TABLE ((int)&g_course_lap_data_field49600_0xc1c0) /* base address of the base records */


/* course_get_record_base */

/* "(i): 0x681828 + i * 0x4C" */

/* Address of base record number index. */
int course_get_record_base(int index)
{
  return index * COURSE_RECORD_STRIDE + COURSE_RECORD_BASE_TABLE;
}
