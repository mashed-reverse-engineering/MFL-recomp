// cluster-entries: course_warzone_cb1_sub_45db70
/* Installed cluster course_warzone_cb1_sub_45db70 (tools/cluster_install.py from output/cluster_pilot_kb/course_warzone_cb1_sub_45db70.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*00409eb0*/ extern double player_rank_key();
/*0045da60*/ extern int course_warzone_cb1_sub_45da60();
/*00472340*/ extern int sfx_play_player_set_at();
/*004726a0*/ extern int sfx_play_at();
/*0060d9d8*/ extern undefined4 g_course_player_60d9d8[0x1000];
/* course_warzone_cb1_sub_45db70 */

/* single caller course_warzone_cb1 */

void course_warzone_cb1_sub_45db70(void)

{
  float fVar1;
  undefined4 uVar3;
  int iVar4;
  int iVar5;
  int *piVar6;
  float10 fVar7;
  
  iVar5 = 0;
  do {
    fVar7 = (float10)player_rank_key(iVar5);
    fVar1 = (float)fVar7;
    iVar4 = 0;
    piVar6 = g_course_player_60d9d8;
    do {
      if ((((float)piVar6[-1] <= fVar1) && (fVar1 < (float)(piVar6[-1] + 1))) &&
         (*piVar6 == 0)) {
        uVar3 = course_warzone_cb1_sub_45da60(iVar4);
        *piVar6 = 1;
        sfx_play_at(0x12,uVar3);
        sfx_play_at(0x15,uVar3);
        sfx_play_player_set_at(uVar3);
      }
      piVar6 = piVar6 + 3;
      iVar4 = iVar4 + 1;
    } while ((int)piVar6 < (int)g_course_player_60d9d8 + 0x60);
    iVar5 = iVar5 + 1;
  } while (iVar5 < 4);
  return;
}

