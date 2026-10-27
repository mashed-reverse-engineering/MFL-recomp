#include "mfl_tier2.h"
/*005569b0*/ extern struct RpSpline * RpSplineCreate(RwInt32 p0, RwInt32 p1, struct RwV3d *p2);
/*006ad1e4*/ extern undefined4 g_sound_6ad1e4;
#define AMBIENCE_PATH_POINTS (15)
/* sound_spline_create_b */
/* "RpSplineCreate: ambience path of a course" */
/* Builds the ambience spline of the current course: AMBIENCE_PATH_POINTS hardcoded
   (x, y, z) control points, all at y = -2.6, passed to RpSplineCreate. The result is
   stored in g_sound_6ad1e4. Takes no parameters, returns nothing.
   The second RpSplineCreate argument (2) is the spline type (inferred). */
void sound_spline_create_b(void)
{
  struct {
    RwV3d point_0;
    float p1_x;
    float p1_y;
    float p1_z;
    float p2_x;
    float p2_y;
    float p2_z;
    float p3_x;
    float p3_y;
    float p3_z;
    float p4_x;
    float p4_y;
    float p4_z;
    float p5_x;
    float p5_y;
    float p5_z;
    float p6_x;
    float p6_y;
    float p6_z;
    float p7_x;
    float p7_y;
    float p7_z;
    float p8_x;
    float p8_y;
    float p8_z;
    float p9_x;
    float p9_y;
    float p9_z;
    float p10_x;
    float p10_y;
    float p10_z;
    float p11_x;
    float p11_y;
    float p11_z;
    float p12_x;
    float p12_y;
    float p12_z;
    float p13_x;
    float p13_y;
    float p13_z;
    float p14_x;
    float p14_y;
    float p14_z;
  } ambience_path;

  /* Control points, in order: first point is point_0 (RwV3d), then each point is
     three consecutive floats (x, y, z). 0xc0266666 is -2.6f (y of every point). */
  ambience_path.point_0.x = -43.0;
  ambience_path.point_0.y = -2.6;
  ambience_path.point_0.z = 22.0;
  ambience_path.p1_x = -33.0f;
  ambience_path.p1_y = -2.6f;
  ambience_path.p1_z = 40.0f;
  ambience_path.p2_x = -16.0f;
  ambience_path.p2_y = -2.6f;
  ambience_path.p2_z = 53.0f;
  ambience_path.p3_x = 4.0f;
  ambience_path.p3_y = -2.6f;
  ambience_path.p3_z = 53.0f;
  ambience_path.p4_x = 22.0f;
  ambience_path.p4_y = -2.6f;
  ambience_path.p4_z = 51.0f;
  ambience_path.p5_x = 31.0f;
  ambience_path.p5_y = -2.6f;
  ambience_path.p5_z = 31.0f;
  ambience_path.p6_x = 30.0f;
  ambience_path.p6_y = -2.6f;
  ambience_path.p6_z = 10.0f;
  ambience_path.p7_x = 25.0f;
  ambience_path.p7_y = -2.6f;
  ambience_path.p7_z = -6.0f;
  ambience_path.p8_x = 27.0f;
  ambience_path.p8_y = -2.6f;
  ambience_path.p8_z = -31.0f;
  ambience_path.p9_x = 20.0f;
  ambience_path.p9_y = -2.6f;
  ambience_path.p9_z = -50.0f;
  ambience_path.p10_x = -2.0f;
  ambience_path.p10_y = -2.6f;
  ambience_path.p10_z = -54.0f;
  ambience_path.p11_x = -34.0f;
  ambience_path.p11_y = -2.6f;
  ambience_path.p11_z = -47.0f;
  ambience_path.p12_x = -43.0f;
  ambience_path.p12_y = -2.6f;
  ambience_path.p12_z = -35.0f;
  ambience_path.p13_x = -42.0f;
  ambience_path.p13_y = -2.6f;
  ambience_path.p13_z = -14.0f;
  ambience_path.p14_x = -37.0f;
  ambience_path.p14_y = -2.6f;
  ambience_path.p14_z = 6.1f;
  g_sound_6ad1e4 = RpSplineCreate(AMBIENCE_PATH_POINTS,2,&ambience_path.point_0);
  return;
}
