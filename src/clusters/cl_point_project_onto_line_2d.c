// cluster-entries: point_project_onto_line_2d
/* Installed cluster point_project_onto_line_2d (tools/cluster_install.py from output/cluster_pilot_zy/point_project_onto_line_2d.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*005e64a4*/ extern float k_f_0;
/*005dd330*/ extern float float_1_0;
/*005dd61c*/ extern float k_f_0_001;
/*0047b590*/ extern void car_get_forward(float *out, int car);
/*004d5410*/ extern RwReal _rwSqrt(RwReal x);

/* Projects point p3 onto the infinite 2D line through p1 and p2 (x/z pairs); when it is closer than `radius` and the car is heading away from p1 towards p3,
   out = p3 pushed to distance `radius` from the line, returns 1; else 0. */
int point_project_onto_line_2d(int car, float *p1, float *p2, float *p3, float radius, float *out)
{
  float forward[3];
  float slope, offset, dx, dy, dist2, inv_len;
  double foot_x, foot_y;

  if (p1[0] - p2[0] == k_f_0) {
    foot_x = p1[0];
    foot_y = p3[1];
  } else {
    slope = (p1[1] - p2[1]) / (p1[0] - p2[0]);
    if (slope == k_f_0) {
      foot_x = p3[0];
      foot_y = p1[1];
    } else {
      offset = p1[1] - slope * p1[0];
      foot_x = ((p3[1] - -(float_1_0 / slope) * p3[0]) - offset) / (slope - -(float_1_0 / slope));
      foot_y = slope * foot_x + offset;
    }
  }
  dx = p3[0] - foot_x;
  dy = p3[1] - foot_y;
  dist2 = dy * dy + dx * dx;
  if (dist2 < radius * radius) {
    car_get_forward(forward, car);
    forward[1] = 0.0f;
    if (k_f_0 < forward[2] * (p3[1] - p1[1]) + forward[0] * (p3[0] - p1[0])) {
      if (k_f_0_001 < dist2) {
        inv_len = float_1_0 / _rwSqrt(dist2);
        out[0] = p3[0] - dx * inv_len * radius;
        out[1] = p3[1] - inv_len * dy * radius;
        return 1;
      }
      out[0] = p3[0] - dx * radius;
      out[1] = p3[1] - dy * radius;
      return 1;
    }
  }
  return 0;
}

