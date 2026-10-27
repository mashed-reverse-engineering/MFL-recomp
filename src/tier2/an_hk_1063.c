#include "mfl_tier2.h"
/*00481ec0*/ extern char *player_marker_get_clump();
/*00481ee0*/ extern int player_marker_set_enabled();
/*004866d0*/ extern int fx_emitter_spawn_b_default();
/*00499b40*/ extern int explosion_decal();
/*004b2bbd*/ extern int __cdecl FID_conflict__wprintf(char *p0, ...);
/*008bf060*/ extern undefined4 g_course_warzone_8bf060;
/* course_warzone_cb1_sub_45da60 */

/* single caller course_warzone_cb1_sub_45db70 */

float * course_warzone_cb1_sub_45da60(int param_1)

{
  float *pfVar1;
  char *iVar2;
  struct {
    undefined4 uStack_c;
    undefined4 uStack_8;
    undefined4 uStack_4;
  } frame;
  
  iVar2 = player_marker_get_clump(param_1);
  iVar2 = *(char **)(iVar2 + 4);
  frame.uStack_c = 0;
  frame.uStack_8 = 0x3f800000;
  frame.uStack_4 = 0;
  pfVar1 = (float *)(iVar2 + 0x40);
  FID_conflict__wprintf
            ("trigger explosion at (%.3f, %.3f, %.3f\n",(double)*pfVar1,
             (double)*(float *)(iVar2 + 0x44),(double)*(float *)(iVar2 + 0x48));
  course_effect_spawn_at_helper_45c8f0(pfVar1,0x41f00000);
  fx_emitter_spawn_b_default(pfVar1,&frame.uStack_c,0x42340000,0x40000000);
  course_warzone_cb1_sub_45da00((&g_course_warzone_8bf060)[param_1],iVar2 + 0x10);
  explosion_decal(pfVar1,0x40000000,0x3ff33333);
  player_marker_set_enabled(param_1,0);
  return pfVar1;
}


