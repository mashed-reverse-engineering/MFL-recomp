#include "mfl_tier2.h"
/*00544430*/ extern int FUN_00544430(undefined4 p0);
/*005ddacc*/ extern float k_f_360;
#define ZONE_FRAME_ID_OFFSET (0x50)
#define ZONE_POS_X (0x38)
#define ZONE_FRAME_PTR_OFFSET (0x94)
#define ZONE_PREV_POS_X (0x84) /* (inferred) */
#define ZONE_PREV_POS_Y (0x88) /* (inferred) */
#define ZONE_PREV_POS_Z (0x8c) /* (inferred) */
#define ZONE_DELTA_X (0xa8) /* (inferred) */
#define ZONE_DELTA_Y (0xac) /* (inferred) */
#define ZONE_DELTA_Z (0xb0) /* (inferred) */
#define FRAME_POS_X (0x70)
#define FRAME_SUBBLOCK_OFFSET (0x40) /* (inferred) */
/* course_zone_read_frame_state */
/* "copies the frame position (0x70..0x78) into the zone, computes angular delta *360" */
/*
 * Reads the course zone's current frame and copies its position into the zone.
 * zone: course zone state (char *, no struct known yet). Its frame id is at ZONE_FRAME_ID_OFFSET.
 * Also stores a pointer to the frame's sub-block, then the per-axis change since the previous
 * position, scaled by k_f_360 (inferred: degrees). Returns nothing; results go into the zone.
 */
void course_zone_read_frame_state(char *zone)
{
  char *frame_addr;
  char *sub;

  /* copy the frame position into the zone */
  frame_addr = FUN_00544430(*(undefined4 *)(zone + ZONE_FRAME_ID_OFFSET));
  *(RwV3d *)(zone + ZONE_POS_X) = *(RwV3d *)(frame_addr + FRAME_POS_X);
  sub = frame_addr + FRAME_SUBBLOCK_OFFSET;
  *(char **)(zone + ZONE_FRAME_PTR_OFFSET) = sub;

  /* delta = new frame position - previous position, then scale by k_f_360 */
  *(float *)(zone + ZONE_DELTA_X) = *(float *)(sub + 0x30) - *(float *)(zone + ZONE_PREV_POS_X);
  *(float *)(zone + ZONE_DELTA_Y) = *(float *)(sub + 0x34) - *(float *)(zone + ZONE_PREV_POS_Y);
  *(float *)(zone + ZONE_DELTA_Z) = *(float *)(sub + 0x38) - *(float *)(zone + ZONE_PREV_POS_Z);
  *(float *)(zone + ZONE_DELTA_X) = *(float *)(zone + ZONE_DELTA_X) * 360.0f;
  *(float *)(zone + ZONE_DELTA_Y) = *(float *)(zone + ZONE_DELTA_Y) * 360.0f;
  *(float *)(zone + ZONE_DELTA_Z) = *(float *)(zone + ZONE_DELTA_Z) * 360.0f;
  return;
}
