#include "mfl_tier2.h"
/*004d1460*/ extern struct RwFrame * RwFrameTranslate(struct RwFrame *p0, struct RwV3d *p1, enum RwOpCombineType p2);
/*004d1640*/ extern struct RwFrame * RwFrameRotate(struct RwFrame *p0, struct RwV3d *p1, RwReal p2, enum RwOpCombineType p3);
/*004d16e0*/ extern struct RwFrame * RwFrameSetIdentity(struct RwFrame *p0);
/*00668c18*/ extern struct CourseWorld g_course_world;
/*00675668*/ extern struct CourseLap g_course_lap_data;
/*00674d18*/ extern char g_course_world_field8_0xc100[];
/* course_sky_clumps_update */

/* "(camera matrix): positions and rotates the sky clumps around the camera" */

void course_sky_clumps_update(char *param_1)

{
  RwFrame *frame;
  uint *puVar1;
  RwV3d local_c;
  
  local_c.x = 0.0;
  local_c.y = 1.0;
  local_c.z = 0.0;
  puVar1 = g_course_world.sky_clumps;
  do {
    if (*puVar1 != 0) {
      frame = *(RwFrame **)(*puVar1 + 4);
      RwFrameSetIdentity(frame);
      RwFrameRotate(frame,&local_c,*(float *)&g_course_lap_data.split_sectors[0xf],rwCOMBINEREPLACE);
      RwFrameTranslate(frame,(RwV3d *)(param_1 + 0x30),rwCOMBINEPOSTCONCAT);
    }
    puVar1 = puVar1 + 1;
  } while ((char *)puVar1 < ((char *)&g_course_world_field8_0xc100));
  return;
}


