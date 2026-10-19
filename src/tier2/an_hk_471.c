#include "mfl_tier2.h"
/*005569b0*/ extern struct RpSpline * RpSplineCreate(RwInt32 p0, RwInt32 p1, struct RwV3d *p2);
/*006ad1e4*/ extern undefined4 g_sound_6ad1e4;
#define SPLINE_NUM_CTRL_POINTS (11)
/* sound_spline_create_a */

/* "RpSplineCreate: ambience path of a course"
 *
 * Builds the sound ambience spline for the course: a fixed set of
 * SPLINE_NUM_CTRL_POINTS control points (x, y, z floats) on the stack,
 * passed to RpSplineCreate (spline type 1, inferred) and stored in
 * g_sound_6ad1e4. Takes no parameters, returns nothing.
 */

void sound_spline_create_a(void)

{
  struct {
    RwV3d p[11];
  } ctrl;

  ctrl.p[0].x = 21.0f;
  ctrl.p[0].y = 0.4f;
  ctrl.p[0].z = 75.0f;
  ctrl.p[1].x = 25.0f;
  ctrl.p[1].y = 0.4f;
  ctrl.p[1].z = 50.0f;
  ctrl.p[2].x = 25.0f;
  ctrl.p[2].y = 0.4f;
  ctrl.p[2].z = 25.0f;
  ctrl.p[3].x = 29.6f;
  ctrl.p[3].y = 0.4f;
  ctrl.p[3].z = 6.4f;
  ctrl.p[4].x = 26.0f;
  ctrl.p[4].y = 0.4f;
  ctrl.p[4].z = -20.5f;
  ctrl.p[5].x = 20.5f;
  ctrl.p[5].y = 0.4f;
  ctrl.p[5].z = -31.0f;
  ctrl.p[6].x = -8.0f;
  ctrl.p[6].y = 0.4f;
  ctrl.p[6].z = -41.0f;
  ctrl.p[7].x = -32.0f;
  ctrl.p[7].y = 0.4f;
  ctrl.p[7].z = -20.0f;
  ctrl.p[8].x = -37.0f;
  ctrl.p[8].y = 0.4f;
  ctrl.p[8].z = -18.7f;
  ctrl.p[9].x = -18.0f;
  ctrl.p[9].y = 0.4f;
  ctrl.p[9].z = 3.3f;
  ctrl.p[10].x = -22.5f;
  ctrl.p[10].y = 0.4f;
  ctrl.p[10].z = 30.0f;
  g_sound_6ad1e4 = RpSplineCreate(SPLINE_NUM_CTRL_POINTS,1,&ctrl.p[0]);
  return;
}


