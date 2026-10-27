#include "mfl_tier2.h"
/*006c288c*/ extern undefined4 g_car_6c288c;
/*006c2894*/ extern float g_car_6c2894;
/*006c2898*/ extern undefined4 g_course_open_6c2898;
/*006c289c*/ extern float g_car_6c289c;
/*006c28b8*/ extern float g_car_6c28b8;
/*006c28bc*/ extern float g_car_6c28bc;
/*006c28a0*/ extern float g_car_6c28a0;
/*006c28b4*/ extern int g_car_6c28b4;
/*006c28a4*/ extern float g_car_6c28a4;
/*006c28b0*/ extern int g_car_6c28b0;
/* course_cell_bounds_from_position */
/* "computes cell indices and fills the cell min/max bounds" */
/* Finds the course grid cell that contains a world position and writes that cell's
 * world-space bounds.
 *   position: object whose float at +0 is world x and at +8 is world z (inferred).
 *   bounds:   float[6] out. [0..2] = max x, y, z; [3..5] = min x, y, z.
 *             y limits come from g_car_6c288c (max) and g_course_open_6c2898 (min) (inferred).
 *   cell_x, cell_z: out. Cell indices along x and z. Not clamped to the grid,
 *             so positions outside the course give out-of-range indices.
 * Cell index = (int)(((coord - origin) / span) * cell_count), truncated toward zero.
 * Origin, span and cell size per axis come from the g_car_6c28xx globals (inferred).
 * No loops or branches. */
void course_cell_bounds_from_position(void *position,float *bounds,int *cell_x,int *cell_z)
{
  int cell;

  cell = ((int)(((*(float *)((char *)position + 0) - g_car_6c2894) / g_car_6c28a0) * g_car_6c28b4));
  *cell_x = cell;
  cell = ((int)(((*(float *)((char *)position + 8) - g_car_6c289c) / g_car_6c28a4) * g_car_6c28b0));
  *cell_z = cell;
  bounds[3] = (float)*cell_x * g_car_6c28bc + g_car_6c2894;
  ((int *)bounds)[4] = g_course_open_6c2898;
  bounds[5] = (float)*cell_z * g_car_6c28b8 + g_car_6c289c;
  *bounds = (float)(*cell_x + 1) * g_car_6c28bc + g_car_6c2894;
  ((int *)bounds)[1] = g_car_6c288c;
  bounds[2] = (float)(*cell_z + 1) * g_car_6c28b8 + g_car_6c289c;
  return;
}
