#include "mfl_tier2.h"
/*00475760*/ extern char *aux_camera_get_position();
/* point_lerp_to_aux_camera */

/* "(a, out, t): out = a + (aux camera position - a) * t" */

void point_lerp_to_aux_camera(float *param_1,float *param_2,float param_3)

{
  float dx;
  float dy;
  double dz;
  volatile float py;      /* volatile: forces the float store of the product (x87 rounding) */
  volatile float pz;
  float *pfVar7;
  char *aux_position;

  pfVar7 = (float *)aux_camera_get_position(0);
  dx = *pfVar7 - *param_1;
  aux_position = aux_camera_get_position(0);
  dy = *(float *)(aux_position + 4) - param_1[1];
  aux_position = aux_camera_get_position(0);
  /* x87 rounding as in the original: dx, dy and the y / z products are stored as float, dz and the x product stay unrounded */
  dz = (double)*(float *)(aux_position + 8) - (double)param_1[2];
  py = dy * param_3;
  pz = (float)(dz * param_3);
  *param_2 = dx * param_3 + *param_1;
  param_2[1] = py + param_1[1];
  param_2[2] = pz + param_1[2];
  return;
}
