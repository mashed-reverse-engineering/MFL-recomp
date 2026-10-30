// cluster-entries: course_highway_cb1_impl
/* Installed cluster course_highway_cb1_impl (tools/cluster_install.py from output/cluster_pilot_kb/course_highway_cb1_impl.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*004867f0*/ extern int particles_set_position();
/*00486840*/ extern int particles_set_color();
/*00486880*/ extern void gfx_set_gfx_6c50c8(float angle);
/*00486b50*/ extern int particles_add();
/*00486c40*/ extern int particles_flush();
/*005ddafc*/ extern float k_f_0_05;
/*005de27c*/ extern float k_f_1_5707964;
/*0060d8a4*/ extern float g_course_particle_60d8a4;
/*0060d8ac*/ extern unsigned int g_data_60d8ac; /* RGBA colour */
/*006a07d8*/ extern undefined g_course_smoke_6a07d8;
/*006a1418*/ extern undefined4 g_course_particle_6a1418;
/*007ef7f4*/ extern float g_tick_seconds;
#define COLOUR_ALPHA_SHIFT (0x18)
#define SMOKE_AGE_ARRAY_BYTES (0x400) /* 256 float ages at g_course_particle_6a1418 */
#define SMOKE_POS_Y_START ((int)&g_course_smoke_6a07d8 + 0x44) /* y of the first of 256 smoke positions (3 floats each, inferred) */
/* course_highway_cb1_impl */

/* tail-jump target of thunk course_highway_cb1_sub_453bf0 */

/* Per-frame highway smoke: for each of 256 particles still within its lifetime, emit a fading, rotating particle at its position, then advance its age and raise it (inferred). */
void course_highway_cb1_impl(void)

{
  undefined1 alpha_byte;
  float *age_ptr;
  float *pos_y_ptr;
  struct {
    undefined4 color;
    float life_frac;
    uint base_alpha;
  } frame;
  
  pos_y_ptr = (float *)SMOKE_POS_Y_START;
  age_ptr = (float *)&g_course_particle_6a1418;
  do {
    if (*age_ptr <= g_course_particle_60d8a4) {
      frame.base_alpha = g_data_60d8ac >> COLOUR_ALPHA_SHIFT;
      frame.life_frac = *age_ptr / g_course_particle_60d8a4;
      frame.color = g_data_60d8ac;
      alpha_byte = ((int)((1.0f - frame.life_frac) * frame.base_alpha));
      frame.color = CONCAT13(alpha_byte,(undefined3)frame.color);
      particles_set_position(pos_y_ptr + -1);
      particles_set_color(&frame.color);
      gfx_set_gfx_6c50c8(frame.life_frac * k_f_1_5707964);
      particles_add(&g_course_smoke_6a07d8);
      *age_ptr = g_tick_seconds + *age_ptr;
      *pos_y_ptr = g_tick_seconds * k_f_0_05 + *pos_y_ptr;
    }
    age_ptr = age_ptr + 1;
    pos_y_ptr = pos_y_ptr + 3;
  } while ((int)age_ptr < (int)&g_course_particle_6a1418 + SMOKE_AGE_ARRAY_BYTES);
  particles_flush(&g_course_smoke_6a07d8);
  return;
}

