// cluster-entries: rwp_cars_sphere_points_generate
/* Installed cluster rwp_cars_sphere_points_generate (tools/cluster_install.py from output/cluster_pilot_uc3/rwp_cars_sphere_points_generate.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
extern double sin(double x);
extern double cos(double x);
extern double sqrt(double x);
#pragma intrinsic(sin, cos, sqrt)
/*0056c5d0*/ extern float *FUN_0056c5d0(void *body, int arg, float *point, float *out);
/*0058d0c0*/ extern int __cdecl FUN_0058d0c0(unsigned int count, float *points, int a, int b);

#define SPHERE_POINTS 120
extern RwV3d g_rwp_cars_701ca0[SPHERE_POINTS];      /* unit sphere sample directions, built on first use */
extern unsigned int g_rwp_cars_702240;              /* number of directions in the table */

/* Builds (once) the table of 120 unit directions spread over the sphere by a spiral of latitude rings, then, for the body `body` (type word at +0x5c:
   1 scales the offset tiny, 4 limits the radius to >= 0.005), asks FUN_0056c5d0 for each direction's support point and writes
   point = radius * direction + scale * support into one local list, which is passed on to FUN_0058d0c0 (its result is returned; 0 when a support
   point could not be found). */
int rwp_cars_sphere_points_generate(void *body, float scale, int arg)
{
  float step;
  float sin_theta, cos_theta, ring_step, ring_end;
  double theta, phase, angle;
  float radius;
  float points[SPHERE_POINTS * 3];
  float *out;
  float *result;
  unsigned int count;
  unsigned int i;
  short type;
  float old0, old1, old2;

  if (g_rwp_cars_702240 == 0) {
    step = (float)(1.78f / sqrt(120.0));
    phase = 0.0f;
    theta = step;
    count = 0;
    while (theta < 3.1415927f) {
      sin_theta = (float)sin(theta);
      cos_theta = (float)cos(theta);
      ring_step = step / (!(sin_theta >= 0.0f) ? -sin_theta : sin_theta);
      ring_end = 6.2831855f - ring_step;
      angle = phase;
      while (angle < ring_end) {
        if (count >= SPHERE_POINTS) {
          break;
        }
        g_rwp_cars_701ca0[count].x = (float)(cos(angle) * sin_theta);
        g_rwp_cars_701ca0[count].y = (float)(sin(angle) * sin_theta);
        g_rwp_cars_701ca0[count].z = cos_theta;
        count++;
        angle = ring_step + ring_step + angle;
      }
      phase = step - phase;
      theta = step + step + theta;
    }
    g_rwp_cars_702240 = count;
  }
  radius = *(float *)((char *)body + 0x4c) * scale;
  type = **(short **)((char *)body + 0x5c);
  if (type == 1) {
    scale = scale * 2.9387361e-36f;
  }
  if (type == 4 && radius < 0.005f) {
    radius = 0.005f;
  }
  out = points;
  for (i = 0; i < g_rwp_cars_702240; i++) {
    result = FUN_0056c5d0(body, arg, &g_rwp_cars_701ca0[i].x, out);
    if (result == 0) {
      return 0;
    }
    out = result + 3;
    old0 = result[0];
    result[0] = scale * old0;
    old1 = result[1];
    result[1] = scale * old1;
    old2 = result[2];
    result[2] = scale * old2;
    result[0] = radius * g_rwp_cars_701ca0[i].x + scale * old0;
    result[1] = g_rwp_cars_701ca0[i].y * radius + scale * old1;
    result[2] = g_rwp_cars_701ca0[i].z * radius + scale * old2;
  }
  return FUN_0058d0c0(g_rwp_cars_702240, points, 0, 0);
}

