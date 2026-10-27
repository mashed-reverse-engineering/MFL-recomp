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
/* course_highway_cb1_impl */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* tail-jump target of thunk course_highway_cb1_sub_453bf0 */

void course_highway_cb1_impl(void)

{
  undefined1 uVar1;
  float *pfVar2;
  float *pfVar3;
  struct {
    undefined4 uStack_c;
    float fStack_8;
    uint uStack_4;
  } frame;
  
  pfVar3 = (float *)0x6a081c;
  pfVar2 = (float *)&g_course_particle_6a1418;
  do {
    if (*pfVar2 < g_course_particle_60d8a4 != (*pfVar2 == g_course_particle_60d8a4)) {
      frame.uStack_4 = g_data_60d8ac >> 0x18;
      frame.fStack_8 = *pfVar2 / g_course_particle_60d8a4;
      frame.uStack_c = g_data_60d8ac;
      uVar1 = ((int)((1.0f - frame.fStack_8) * frame.uStack_4));
      frame.uStack_c = CONCAT13(uVar1,(undefined3)frame.uStack_c);
      particles_set_position(pfVar3 + -1);
      particles_set_color(&frame.uStack_c);
      gfx_set_gfx_6c50c8(frame.fStack_8 * k_f_1_5707964);
      particles_add(&g_course_smoke_6a07d8);
      *pfVar2 = g_tick_seconds + *pfVar2;
      *pfVar3 = g_tick_seconds * k_f_0_05 + *pfVar3;
    }
    pfVar2 = pfVar2 + 1;
    pfVar3 = pfVar3 + 3;
  } while ((int)pfVar2 < (int)&g_course_particle_6a1418 + 0x400);
  particles_flush(&g_course_smoke_6a07d8);
  return;
}

