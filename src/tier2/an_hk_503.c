#include "mfl_tier2.h"
/*00675668*/ extern struct CourseLap g_course_lap_data;
/* course_set_split_sector_7 */

/* "g_course_lap_data.split_sectors[7] = value (camera in the collision world flag)" */

void __cdecl
course_set_split_sector_7
          (int param_1,char *param_2,char *param_3,char *param_4,char *param_5)

{
  g_course_lap_data.split_sectors[7] = param_1;
  return;
}


