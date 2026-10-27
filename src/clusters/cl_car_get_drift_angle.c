// cluster-entries: car_get_drift_angle
/* Installed cluster car_get_drift_angle (tools/cluster_install.py from output/cluster_pilot_zy/car_get_drift_angle.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*008b06e0*/ extern struct Car player_car[16];
/*005dd364*/ extern float k_f_n1;
/*005dd330*/ extern float float_1_0;
/*005ddc30*/ extern double k_f_n2_5ddc30;
/*004d5290*/ extern RwReal RwV3dNormalize(struct RwV3d *out, struct RwV3d *in);
/*004b3284*/ extern double FUN_004b3284(double x);

/* Angle in degrees between car `index`'s heading (forward) and its velocity direction, written to *angle (0 when the car is not moving or index >= 16); returns 1 when an angle was computed. */
int car_get_drift_angle(float *angle, unsigned int index)
{
  float dir[3];
  float cos_a;

  *angle = 0.0f;
  if (index < MAX_CARS) {
    if (player_car[index].velocity[0] != 0.0f || player_car[index].velocity[1] != 0.0f || player_car[index].velocity[2] != 0.0f) {
      RwV3dNormalize((struct RwV3d *)dir, (struct RwV3d *)player_car[index].velocity);
      cos_a = player_car[index].forward[1] * dir[1] + player_car[index].forward[0] * dir[0] + player_car[index].forward[2] * dir[2];
      if (cos_a < k_f_n1) {
        cos_a = k_f_n1;
      } else if (cos_a > float_1_0) {
        cos_a = float_1_0;
      }
      *angle = (float)(FUN_004b3284(cos_a) * k_f_n2_5ddc30);
      return 1;
    }
  }
  return 0;
}

