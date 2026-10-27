#include "mfl_tier2.h"
/*00475760*/ extern char *aux_camera_get_position();
/* point_lerp_to_aux_camera */

/* "(a, out, t): out = a + (aux camera position - a) * t" */

void point_lerp_to_aux_camera(float *from,float *out,float t)

{
  float dx;
  float dy;
  double dz;
  volatile float py;      /* volatile: forces the float store of the product (x87 rounding) */
  volatile float pz;
  float *cam_pos;
  char *aux_position;

  cam_pos = (float *)aux_camera_get_position(0);
  dx = *cam_pos - *from;
  aux_position = aux_camera_get_position(0);
  dy = *(float *)(aux_position + 4) - from[1];
  aux_position = aux_camera_get_position(0);
  /* x87 rounding as in the original: dx, dy and the y / z products are stored as float, dz and the x product stay unrounded */
  dz = (double)*(float *)(aux_position + 8) - (double)from[2];
  py = dy * t;
  pz = (float)(dz * t);
  *out = dx * t + *from;
  out[1] = py + from[1];
  out[2] = pz + from[2];
  return;
}
