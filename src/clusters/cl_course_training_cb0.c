// cluster-entries: course_training_cb0
/* Installed cluster course_training_cb0 (tools/cluster_install.py from output/cluster_pilot_kb/course_training_cb0.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
extern char UNK_005df51c[];
extern int _rpUVAnimDictSchema;
/*0040d0d0*/ extern int __cdecl find_texture_dict_b(RwChar *p0);
/*004849e0*/ extern int course_helper_4849e0();
/*00485180*/ extern int course_training_cb0_sub_485180();
/*0049c1a0*/ extern int particles_slot_set_vectors();
/*0049c280*/ extern int fx_snow_ptank_create();
/*0049e170*/ extern int course_training_cb0_sub_49e170(int unused); /* the caller pushes one argument that the function does not read */
/*00569270*/ extern int RtDictSchemaSetCurrentDict();
/*005dd340*/ extern float k_f_0_5;
/*0064d894*/ extern undefined4 g_course_64d894;
/*008bf080*/ extern undefined4 g_course_8bf080;
/*008bf084*/ extern undefined4 g_course_training_8bf084;
/* course_training_cb0 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void course_training_cb0(char *param_1)

{
  undefined4 texture_dict;
  int iVar1;
  int iVar2;
  undefined4 *puVar3;
  struct {
    undefined4 uStack_24;
    float fStack_20;
    undefined4 uStack_1c;
    undefined4 uStack_18;
    undefined4 uStack_14;
    undefined4 uStack_10;
    undefined4 uStack_c;
    undefined4 uStack_8;
    undefined4 uStack_4;
  } frame;
  
  course_training_cb0_sub_49e170(0);
  g_course_training_8bf084 = course_helper_4849e0(&UNK_005df51c,*(undefined4 *)(param_1 + 0xc9d0));
  frame.uStack_24 = 0xc23af5c3;
  frame.fStack_20 = 0.747;
  frame.uStack_1c = 0xc28c245a;
  frame.uStack_c = 0xbbf5c28f;
  frame.uStack_8 = 0x3c75c28f;
  frame.uStack_4 = 0xbbf5c28f;
  frame.uStack_18 = 0x3bf5c28f;
  frame.uStack_14 = 0x3d23d70a;
  frame.uStack_10 = 0x3bf5c28f;
  texture_dict = find_texture_dict_b("smoke");
  fx_snow_ptank_create(0,texture_dict);
  iVar1 = 0;
  iVar2 = 0;
  if (0 < *(int *)(param_1 + 0xc008)) {
    puVar3 = (undefined4 *)(param_1 + 0x1830);
    do {
      frame.uStack_24 = *puVar3;
      frame.uStack_1c = puVar3[2];
      frame.fStack_20 = *(float *)&puVar3[1] - k_f_0_5;
      particles_slot_set_vectors(iVar1,&frame.uStack_24,0x3a83126f,&frame.uStack_c,&frame.uStack_18);
      if (3 < iVar2) {
        iVar1 = iVar1 + 1;
      }
      iVar2 = iVar2 + 1;
      puVar3 = puVar3 + 0x10;
    } while (iVar2 < *(int *)(param_1 + 0xc008));
  }
  g_course_8bf080 = *(undefined4 *)(param_1 + 0xca18);
  RtDictSchemaSetCurrentDict(&_rpUVAnimDictSchema,g_course_8bf080);
  if (g_course_64d894 != 0) {
    course_training_cb0_sub_485180(*(undefined4 *)(param_1 + 0xc13c));
  }
  return;
}

