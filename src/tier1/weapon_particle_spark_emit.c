#include "mfl_tier2.h"
/*00481fd0*/ extern double random_range();
/*00482010*/ extern int random_int_range();
/*004846e0*/ extern int point_lerp_to_aux_camera();
/*004867f0*/ extern int particles_set_position();
/*00486820*/ extern int particles_set_size();
/*00486840*/ extern int particles_set_color();
/*00486880*/ extern int gfx_set_gfx_6c50c8(float p0);      /* defectlint: was `()`: the float was pushed as a double */
/*00486b50*/ extern int particles_add();
/*006a61f0*/ extern undefined g_weapon_6a61f0;
/*006a6230*/ extern undefined4 g_weapon_6a6230;
#define SPARK_GREY_RANGE (0x80)         /* random_int_range upper bound; grey = -1 - r gives 127..255 (inclusive range, inferred) */
#define SPARK_MAX_COUNT (0x80)          /* cap on the spark counter g_weapon_6a6230 (inferred) */
#define SPARK_ALPHA (0xff)              /* alpha byte of the colour word */
#define SPARK_LERP_FRACTION (0x3dcccccd) /* float 0.1: how far the spark moves toward the aux camera (inferred) */
#define SPARK_ANGLE_RANGE (0x43b40000)  /* float 360.0: upper bound of the random angle (inferred) */
/* weapon_rocket_clear_effects */
/* weapon_rocket_reset_all */
/* weapon_particle_spark_emit */
/* "(pos, size): random grey spark particle moving toward the aux camera" */
/* Emits one random grey spark particle at pos, moving toward the aux camera (inferred).
 * pos is the spark's world position; size is written to both floats of the size pair
 * (width and height). Does nothing once the spark counter g_weapon_6a6230 has reached
 * SPARK_MAX_COUNT. Returns nothing. */
void weapon_particle_spark_emit(undefined4 pos,float size)
{
  char grey;
  double angle;
  float size_pair [2];
  undefined1 lerped_pos [12];
  RwRGBA color;

  size_pair[0] = size;
  size_pair[1] = size;
  grey = random_int_range(0,SPARK_GREY_RANGE);
  if ((int)g_weapon_6a6230 < SPARK_MAX_COUNT) {
    grey = -1 - grey;
    color.red = grey;
    color.green = grey;
    color.blue = grey;
    color.alpha = SPARK_ALPHA;
    particles_set_color(&color);
    particles_set_size(size_pair);
    point_lerp_to_aux_camera(pos,lerped_pos,SPARK_LERP_FRACTION);
    particles_set_position(lerped_pos);
    angle = (double)random_range(0,SPARK_ANGLE_RANGE);
    gfx_set_gfx_6c50c8((float)angle);
    particles_add(&g_weapon_6a61f0);
    g_weapon_6a6230 = g_weapon_6a6230 + 1;
  }
  return;
}
