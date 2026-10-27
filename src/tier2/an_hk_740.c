#include "mfl_tier2.h"
/*0044d1f0*/ extern undefined ai_grid_cell_lookup_0044d1f0(undefined4 p0, undefined4 p1);
/*0047b520*/ extern int car_current_matrix();
/*0047b590*/ extern int car_get_forward();
/*004d54d0*/ extern RwReal RwV2dLength(struct RwV2d *p0);
/*005dd330*/ extern float float_1_0;
/*005dd628*/ extern float k_f_0_25;
/*005ddc28*/ extern float k_f_0;

/* RwMatrix.pos.x and RwMatrix.pos.z byte offsets (RwMatrixTag: right, flags, up, pad, at, pad, pos) */
#define RWMATRIX_POS_X_OFFSET (0x30)
#define RWMATRIX_POS_Z_OFFSET (0x38)
/* ai_bot_obstacle_ahead */

/*
 * Returns 1 if the grid shows an obstacle on the straight line from the car's position (matrix pos x/z)
 * to 2 forward vectors ahead of it, else 0. The line is sampled every k_f_0_25 units; a sample whose
 * grid cell has kind 0 or 3 counts as blocked (inferred). car_index: car index for car_current_matrix /
 * car_get_forward (inferred). (was: "inferred: tests the point 2 forward vectors ahead of the car")
 */

undefined4 __cdecl ai_bot_obstacle_ahead(undefined4 car_index)

{
  float inv_probe_length;
  char cell_kind;
  RwReal probe_length;
  struct {
    float distance_along;
    undefined1 unused_pad[8];
    char *car_matrix;
    float origin_x;
    float origin_z;
    RwV2d delta;
    float sample_x;
    float sample_z;
    float forward_x;
    float forward_y;
    float forward_z;
  } ray;

  car_current_matrix(&ray.car_matrix,car_index);
  ray.origin_x = *(float *)(ray.car_matrix + RWMATRIX_POS_X_OFFSET);
  ray.origin_z = *(float *)(ray.car_matrix + RWMATRIX_POS_Z_OFFSET);
  car_get_forward(&ray.forward_x,car_index);
  ray.forward_x = ray.forward_x + ray.forward_x;
  ray.distance_along = 0.0;
  ray.forward_y = ray.forward_y + ray.forward_y;
  ray.forward_z = ray.forward_z + ray.forward_z;
  ray.delta.x = (ray.forward_x + ray.origin_x) - ray.origin_x;
  ray.delta.y = (ray.forward_z + ray.origin_z) - ray.origin_z;
  probe_length = RwV2dLength(&ray.delta);
  /* only probe when the segment has a non-zero length */
  if (k_f_0 < probe_length) {
    inv_probe_length = float_1_0 / probe_length;
    /* walk from the car along the unit direction in steps of k_f_0_25 until the segment end */
    do {
      ray.sample_x = inv_probe_length * ray.delta.x * ray.distance_along + ray.origin_x;
      ray.sample_z = inv_probe_length * ray.delta.y * ray.distance_along + ray.origin_z;
      cell_kind = ai_grid_cell_lookup_0044d1f0(*(int *)&ray.sample_x, *(int *)&ray.sample_z);
      /* cell kinds 0 and 3 (inferred) block the way: report an obstacle at once */
      if ((cell_kind == '\0') || (cell_kind == '\x03')) {
        return 1;
      }
      ray.distance_along = ray.distance_along + k_f_0_25;
    } while (ray.distance_along < probe_length);
  }
  return 0;
}


