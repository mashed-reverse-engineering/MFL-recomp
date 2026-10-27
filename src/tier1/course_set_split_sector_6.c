#include "mfl_tier2.h"
/*00675668*/ extern struct CourseLap g_course_lap_data;

#define SPLIT_SECTOR_SLOT 6 /* index of the sixth split sector in split_sectors[] */


/* course_set_split_sector_6 */

/* "g_course_lap_data.split_sectors[6] = value" */

/* Set the split-sector slot SPLIT_SECTOR_SLOT of the course lap data. */
void course_set_split_sector_6(int sector_value)
{
  g_course_lap_data.split_sectors[SPLIT_SECTOR_SLOT] = sector_value;
  return;
}
