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
    RwV3d local_b4;
    float local_a8;
    float local_a4;
    float local_a0;
    float local_9c;
    float local_98;
    float local_94;
    float local_90;
    float local_8c;
    float local_88;
    float local_84;
    float local_80;
    float local_7c;
    float local_78;
    float local_74;
    float local_70;
    float local_6c;
    float local_68;
    float local_64;
    float local_60;
    float local_5c;
    float local_58;
    float local_54;
    float local_50;
    float local_4c;
    float local_48;
    float local_44;
    float local_40;
    float local_3c;
    float local_38;
    float local_34;
    float local_30;
    float local_2c;
    float local_28;
    float local_24;
    float local_20;
    float local_1c;
    float local_18;
    float local_14;
    float local_10;
    float local_c;
    float local_8;
    float local_4;
  } ambience_path;

  /* Control points, in order: first point is local_b4 (RwV3d), then each point is
     three consecutive floats (x, y, z). 0xc0266666 is -2.6f (y of every point). */
  ambience_path.local_b4.x = -43.0;
  ambience_path.local_b4.y = -2.6;
  ambience_path.local_b4.z = 22.0;
  ambience_path.local_a8 = -33.0f;
  ambience_path.local_a4 = -2.6f;
  ambience_path.local_a0 = 40.0f;
  ambience_path.local_9c = -16.0f;
  ambience_path.local_98 = -2.6f;
  ambience_path.local_94 = 53.0f;
  ambience_path.local_90 = 4.0f;
  ambience_path.local_8c = -2.6f;
  ambience_path.local_88 = 53.0f;
  ambience_path.local_84 = 22.0f;
  ambience_path.local_80 = -2.6f;
  ambience_path.local_7c = 51.0f;
  ambience_path.local_78 = 31.0f;
  ambience_path.local_74 = -2.6f;
  ambience_path.local_70 = 31.0f;
  ambience_path.local_6c = 30.0f;
  ambience_path.local_68 = -2.6f;
  ambience_path.local_64 = 10.0f;
  ambience_path.local_60 = 25.0f;
  ambience_path.local_5c = -2.6f;
  ambience_path.local_58 = -6.0f;
  ambience_path.local_54 = 27.0f;
  ambience_path.local_50 = -2.6f;
  ambience_path.local_4c = -31.0f;
  ambience_path.local_48 = 20.0f;
  ambience_path.local_44 = -2.6f;
  ambience_path.local_40 = -50.0f;
  ambience_path.local_3c = -2.0f;
  ambience_path.local_38 = -2.6f;
  ambience_path.local_34 = -54.0f;
  ambience_path.local_30 = -34.0f;
  ambience_path.local_2c = -2.6f;
  ambience_path.local_28 = -47.0f;
  ambience_path.local_24 = -43.0f;
  ambience_path.local_20 = -2.6f;
  ambience_path.local_1c = -35.0f;
  ambience_path.local_18 = -42.0f;
  ambience_path.local_14 = -2.6f;
  ambience_path.local_10 = -14.0f;
  ambience_path.local_c = -37.0f;
  ambience_path.local_8 = -2.6f;
  ambience_path.local_4 = 6.1f;
  g_sound_6ad1e4 = RpSplineCreate(AMBIENCE_PATH_POINTS,2,&ambience_path.local_b4);
  return;
}
