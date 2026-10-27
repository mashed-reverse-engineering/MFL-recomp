#include "mfl_tier2.h"
/*00675668*/ extern struct CourseLap g_course_lap_data;
/* course_set_split_sector_7 */
/* "g_course_lap_data.split_sectors[7] = value (camera in the collision world flag)" */
void __cdecl course_set_split_sector_7(int value,char *unused_2,char *unused_3,char *unused_4,char *unused_5)
{
  g_course_lap_data.split_sectors[7] = value;
  return;
}
