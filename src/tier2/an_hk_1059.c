#include "mfl_tier2.h"
/*00472520*/ extern int sfx_play_group_voice_at();
/*004726a0*/ extern int sfx_play_at();
/*004866b0*/ extern int fx_emitter_spawn_a_default();
/*004d5290*/ extern RwReal RwV3dNormalize(struct RwV3d *p0, struct RwV3d *p1);
/* course_warzone_cb1_sub_45d630 */

/* single caller course_warzone_cb1_sub_45d720 */

void course_warzone_cb1_sub_45d630(int *param_1,float param_2)

{
  float fVar1;
  RwV3d RStack_c;
  
  fVar1 = *(float *)&param_1[1];
  *(float *)&param_1[1] = (param_2 + fVar1);
  if (*(float *)&param_1[2] < param_2 + fVar1) {
    RwV3dNormalize(&RStack_c,(RwV3d *)(*param_1 + 0x20));
    fx_emitter_spawn_a_default(*param_1 + 0x30,&RStack_c,0x42340000);
    course_effect_spawn_at_helper_45c8f0(*param_1 + 0x30,0x41200000);
    param_1[1] = 0;
    sfx_play_at(0x12,*param_1 + 0x30);
    sfx_play_group_voice_at(*param_1 + 0x30,0);
  }
  return;
}


