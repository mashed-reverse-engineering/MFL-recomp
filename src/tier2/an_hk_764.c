#include "mfl_tier2.h"
/*0045c820*/ extern undefined __cdecl course_particle_init_random(undefined4 p0);
/*004d0ff0*/ extern RwMatrix * RwFrameGetLTM(struct RwFrame *p0);
/*004d5670*/ extern struct RwV3d * RwV3dTransformPoints(struct RwV3d *p0, struct RwV3d *p1, RwInt32 p2, RwMatrix *p3);
/*005dd378*/ extern float k_f_0_1;
/*005ddc28*/ extern float k_f_0;
#define ZONE_OBJECT_PTR_OFFSET (0x8) /* (inferred) zone + 8 holds a pointer to an object */
#define OBJECT_FRAME_PTR_OFFSET (0x4) /* (inferred) object + 4 holds the RwFrame */
#define ZONE_CORNER_X_OFFSET (0x14) /* (inferred) x of both zone corners */
#define ZONE_CORNER_Y_OFFSET (0x24) /* (inferred) y of both zone corners */
#define ZONE_CORNER0_Z_OFFSET (0x28) /* (inferred) z of corner 0 */
#define ZONE_CORNER1_Z_OFFSET (0x1c) /* (inferred) z of corner 1 */
#define ZONE_CORNER_COUNT (2)
#define ZONE_SAMPLE_COUNT (8)
#define ZONE_SAMPLE_HEIGHT (-2.3) /* (inferred) height the samples are placed at */
/* course_zone_corners_to_world */

/* "transforms the zone corner points with the frame LTM and derives step sizes (0.1)" */

/*
 * Purpose (inferred): places particle spawn points along one edge of a course zone.
 * Takes both zone corners (zone + ZONE_CORNER_*_OFFSET, relative to the zone's RwFrame), moves them into
 * world space with the frame LTM, flattens them to ZONE_SAMPLE_HEIGHT and calls course_particle_init_random
 * on ZONE_SAMPLE_COUNT points from corner 1 toward corner 0, then on corner 0.
 * zone: int address of the zone record (not a typed pointer); the register is reused as the loop counter.
 * Returns nothing.
 */
void course_zone_corners_to_world(int zone)

{
  volatile float z_step_offset; /* volatile in the original build: kept in memory (inferred) */
  float step_index;
  RwMatrix *frame_ltm;
  struct {
    float sample_x;
    float sample_y;
    float sample_z;
    float step_x;
    undefined1 pad_20[4];
    float step_z;
    RwV3d corner0;
    float corner1_x;
    float corner1_y;
    float corner1_z;
  } scratch;

  /* world-space corners: both points (ZONE_CORNER_COUNT) go through the frame LTM */
  frame_ltm = RwFrameGetLTM(*(RwFrame **)(*(char **)(zone + ZONE_OBJECT_PTR_OFFSET) + OBJECT_FRAME_PTR_OFFSET));
  scratch.corner0.x = *(float *)(zone + ZONE_CORNER_X_OFFSET);
  scratch.corner0.y = *(RwReal *)(zone + ZONE_CORNER_Y_OFFSET);
  scratch.corner0.z = *(float *)(zone + ZONE_CORNER0_Z_OFFSET);
  scratch.corner1_y = *(float *)(zone + ZONE_CORNER_Y_OFFSET);
  scratch.corner1_x = *(float *)(zone + ZONE_CORNER_X_OFFSET);
  scratch.corner1_z = *(float *)(zone + ZONE_CORNER1_Z_OFFSET);
  RwV3dTransformPoints(&scratch.corner0,&scratch.corner0,ZONE_CORNER_COUNT,frame_ltm);
  scratch.corner0.y = ZONE_SAMPLE_HEIGHT;
  scratch.corner1_y = ZONE_SAMPLE_HEIGHT;
  scratch.step_x = (scratch.corner0.x - scratch.corner1_x) * k_f_0_1;
  zone = 0;
  scratch.step_z = (scratch.corner0.z - scratch.corner1_z) * k_f_0_1;
  /* sample i = corner 1 + i * step (step = 10% of the corner-to-corner span), i = 0 .. ZONE_SAMPLE_COUNT - 1;
     y moves by k_f_0 per sample (inferred 0, so samples stay at ZONE_SAMPLE_HEIGHT) */
  do {
    step_index = (float)zone;
    scratch.sample_x = scratch.step_x * step_index + scratch.corner1_x;
    z_step_offset = step_index * scratch.step_z;
    scratch.sample_y = step_index * k_f_0 + scratch.corner1_y;
    scratch.sample_z = z_step_offset + scratch.corner1_z;
    course_particle_init_random((undefined4)&scratch.sample_x);
    zone = zone + 1;
  } while (zone < ZONE_SAMPLE_COUNT);
  /* last spawn point: corner 0 itself */
  course_particle_init_random((undefined4)&scratch.corner0);
  return;
}


