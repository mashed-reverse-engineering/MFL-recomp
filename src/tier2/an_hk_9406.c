#include "mfl_tier2.h"
/*004d1460*/ extern struct RwFrame * RwFrameTranslate(struct RwFrame *p0, struct RwV3d *p1, enum RwOpCombineType p2);
/*004d1640*/ extern struct RwFrame * RwFrameRotate(struct RwFrame *p0, struct RwV3d *p1, RwReal p2, enum RwOpCombineType p3);
/*004d16e0*/ extern struct RwFrame * RwFrameSetIdentity(struct RwFrame *p0);
/*00668c18*/ extern struct CourseWorld g_course_world;
/*00675668*/ extern struct CourseLap g_course_lap_data;
/*00674d18*/ extern char g_course_world_field8_0xc100[];
/* course_sky_clumps_update */

/* "(camera matrix): positions and rotates the sky clumps around the camera" */

void course_sky_clumps_update(char *camera_matrix)

{
  RwFrame *frame;
  uint *sky_clump_cursor;
  RwV3d up_axis;
  
  up_axis.x = 0.0;
  up_axis.y = 1.0;
  up_axis.z = 0.0;
  sky_clump_cursor = g_course_world.sky_clumps;
  do {
    if (*sky_clump_cursor != 0) {
      frame = *(RwFrame **)(*sky_clump_cursor + 4);
      RwFrameSetIdentity(frame);
      RwFrameRotate(frame,&up_axis,*(float *)&g_course_lap_data.split_sectors[0xf],rwCOMBINEREPLACE);
      RwFrameTranslate(frame,(RwV3d *)(camera_matrix + 0x30),rwCOMBINEPOSTCONCAT);
    }
    sky_clump_cursor = sky_clump_cursor + 1;
  } while ((char *)sky_clump_cursor < ((char *)&g_course_world_field8_0xc100));
  return;
}


