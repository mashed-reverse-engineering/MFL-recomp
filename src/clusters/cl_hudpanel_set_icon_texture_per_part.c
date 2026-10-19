// cluster-entries: hudpanel_set_icon_texture_per_part hudpanel_load_indicator_update
/* Installed cluster hudpanel_set_icon_texture_per_part (tools/cluster_install.py from output/cluster_pilot7/hudpanel_set_icon_texture_per_part.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*00659b18*/ extern undefined g_game_659b18;
/*0040fdc0*/ extern int race_get_phase();
/*004d1500*/ extern struct RwFrame * RwFrameScale(struct RwFrame *p0, struct RwV3d *p1, enum RwOpCombineType p2);
/*004d15a0*/ extern struct RwFrame * RwFrameTransform(struct RwFrame *p0, RwMatrix *p1, enum RwOpCombineType p2);
/*004d4870*/ extern RwMatrix * RwMatrixTranslate(RwMatrix *p0, struct RwV3d *p1, enum RwOpCombineType p2);
/*005dd40c*/ extern float k_f_1_5;
/*005de288*/ extern float k_f_1_125;
/*00659b30*/ extern undefined4 g_load_659b30;
/*00659ce0*/ extern undefined4 g_load_659ce0;
/*00659cf8*/ extern undefined4 g_game_659cf8;
/*00659cfc*/ extern undefined4 g_game_659cfc;
/* hudpanel_set_icon_texture_per_part */

/* WARNING: Unknown calling convention */
/* "when the index changes assigns the texture colour table entry to each of 12 part materials" */

static int time_trial_panel_times_update_r(int p0);
extern void time_trial_panel_times_update(void);
void hudpanel_set_icon_texture_per_part_n(int in_eax)

{
  int iVar1;
  
  iVar1 = *(int *)(in_eax + 0x1b4);
  if (iVar1 != *(int *)(in_eax + 0x1b8)) {
    *(int *)(in_eax + 0x1b8) = iVar1;
    *(undefined4 *)(**(int **)(*(char **)(*(int *)(in_eax + 0x108) + 0x18) + 0x20) + 4) =
         *(undefined4 *)((char *)&g_game_659b18 + iVar1 * 4);
    *(undefined4 *)(**(int **)(*(char **)(*(int *)(in_eax + 0x10c) + 0x18) + 0x20) + 4) =
         *(undefined4 *)((char *)&g_game_659b18 + *(int *)(in_eax + 0x1b4) * 4);
    *(undefined4 *)(**(int **)(*(char **)(*(int *)(in_eax + 0x110) + 0x18) + 0x20) + 4) =
         *(undefined4 *)((char *)&g_game_659b18 + *(int *)(in_eax + 0x1b4) * 4);
    *(undefined4 *)(**(int **)(*(char **)(*(int *)(in_eax + 0x114) + 0x18) + 0x20) + 4) =
         *(undefined4 *)((char *)&g_game_659b18 + *(int *)(in_eax + 0x1b4) * 4);
    *(undefined4 *)(**(int **)(*(char **)(*(int *)(in_eax + 0x118) + 0x18) + 0x20) + 4) =
         *(undefined4 *)((char *)&g_game_659b18 + *(int *)(in_eax + 0x1b4) * 4);
    *(undefined4 *)(**(int **)(*(char **)(*(int *)(in_eax + 0x11c) + 0x18) + 0x20) + 4) =
         *(undefined4 *)((char *)&g_game_659b18 + *(int *)(in_eax + 0x1b4) * 4);
    *(undefined4 *)(**(int **)(*(char **)(*(int *)(in_eax + 0x120) + 0x18) + 0x20) + 4) =
         *(undefined4 *)((char *)&g_game_659b18 + *(int *)(in_eax + 0x1b4) * 4);
    *(undefined4 *)(**(int **)(*(char **)(*(int *)(in_eax + 0x124) + 0x18) + 0x20) + 4) =
         *(undefined4 *)((char *)&g_game_659b18 + *(int *)(in_eax + 0x1b4) * 4);
    *(undefined4 *)(**(int **)(*(char **)(*(int *)(in_eax + 0x128) + 0x18) + 0x20) + 4) =
         *(undefined4 *)((char *)&g_game_659b18 + *(int *)(in_eax + 0x1b4) * 4);
    *(undefined4 *)(**(int **)(*(char **)(*(int *)(in_eax + 300) + 0x18) + 0x20) + 4) =
         *(undefined4 *)((char *)&g_game_659b18 + *(int *)(in_eax + 0x1b4) * 4);
    *(undefined4 *)(**(int **)(*(char **)(*(int *)(in_eax + 0x178) + 0x18) + 0x20) + 4) =
         *(undefined4 *)((char *)&g_game_659b18 + *(int *)(in_eax + 0x1b4) * 4);
    *(undefined4 *)(**(int **)(*(char **)(*(int *)(in_eax + 0x174) + 0x18) + 0x20) + 4) =
         *(undefined4 *)((char *)&g_game_659b18 + *(int *)(in_eax + 0x1b4) * 4);
  }
  return;
}

/* hudpanel_load_indicator_update */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* "places the load indicator for phase 7 (0.35,0.2, scale 1.5) or otherwise (-0.375,-0.3, 1.125)"
    */

void hudpanel_load_indicator_update(void)

{
  int race_phase;
  struct {
    RwV3d local_64;
    RwV3d local_58;
    RwV3d local_4c;
    RwMatrix local_40;
  } frame;
  
  frame.local_40.flags = frame.local_40.flags | 0x20003;
  frame.local_58.x = -0.375;
  frame.local_58.y = -0.3;
  frame.local_58.z = 1.0;
  frame.local_4c.x = 0.35;
  frame.local_4c.y = 0.2;
  frame.local_4c.z = 1.0;
  frame.local_64.x = 1.0;
  frame.local_64.y = 1.0;
  frame.local_64.z = 1.0;
  g_game_659cfc = g_game_659cf8;
  frame.local_40.at.z = 1.0;
  frame.local_40.up.y = 1.0;
  frame.local_40.right.x = 1.0;
  frame.local_40.up.x = 0.0;
  frame.local_40.right.z = 0.0;
  frame.local_40.right.y = 0.0;
  frame.local_40.at.y = 0.0;
  frame.local_40.at.x = 0.0;
  frame.local_40.up.z = 0.0;
  frame.local_40.pos.z = 0.0;
  frame.local_40.pos.y = 0.0;
  frame.local_40.pos.x = 0.0;
  race_phase = race_get_phase();
  if (race_phase == 7) {
    RwMatrixTranslate(&frame.local_40,&frame.local_4c,rwCOMBINEREPLACE);
    frame.local_64.x = frame.local_64.x * k_f_1_5;
    frame.local_64.y = frame.local_64.y * k_f_1_5;
    frame.local_64.z = frame.local_64.z * k_f_1_5;
  }
  else {
    RwMatrixTranslate(&frame.local_40,&frame.local_58,rwCOMBINEREPLACE);
    frame.local_64.x = frame.local_64.x * k_f_1_125;
    frame.local_64.y = frame.local_64.y * k_f_1_125;
    frame.local_64.z = frame.local_64.z * k_f_1_125;
  }
  RwFrameTransform(g_load_659ce0,&frame.local_40,rwCOMBINEREPLACE);
  RwFrameScale(g_load_659ce0,&frame.local_64,rwCOMBINEPRECONCAT);
  time_trial_panel_times_update_r(&g_load_659b30);
  hudpanel_set_icon_texture_per_part_n(&g_load_659b30);
  return;
}

/* time_trial_panel_times_update_r: C -> original time_trial_panel_times_update (registers ebx) */
__declspec(naked) int time_trial_panel_times_update_r(int p0)
{
    __asm {
        push ebx
        mov ebx, dword ptr [esp + 8]
        call time_trial_panel_times_update
        pop ebx
        ret
    }
}

/* entry thunk at the original address of hudpanel_set_icon_texture_per_part: register inputs (eax) -> natural C hudpanel_set_icon_texture_per_part_n; ret 0 */
__declspec(naked) void hudpanel_set_icon_texture_per_part(void)
{
    __asm {
        push eax
        call hudpanel_set_icon_texture_per_part_n
        add esp, 4
        ret
    }
}
