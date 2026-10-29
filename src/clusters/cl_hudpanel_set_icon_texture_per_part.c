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

/* "when the index changes assigns the texture colour table entry to each of 12 part materials" */

/* HUD panel record, only the fields this function uses: 12 part holders (+0x18 -> material list) and the icon index with its cache */
typedef struct HudIconPanel {
    char unknown_000[0x108];
    int icon_part[10];      /* 0x108 */
    char unknown_130[0x44];
    int icon_part_174;
    int icon_part_178;
    char unknown_17c[0x38];
    int icon_index;         /* 0x1b4 */
    int icon_cached;        /* 0x1b8: last applied icon_index */
} HudIconPanel;
static int time_trial_panel_times_update_r(int p0);
extern void time_trial_panel_times_update(void);
/* panel = HUD panel record whose 12 part material pointers (offsets 0x108..0x12c and 0x174, 0x178) get colour table entry [icon_index] (offset 0x1b4; 0x1b8 caches the last one). */
void hudpanel_set_icon_texture_per_part_n(HudIconPanel *panel)

{
  int icon_index;
  
  icon_index = panel->icon_index;
  if (icon_index != panel->icon_cached) {
    panel->icon_cached = icon_index;
    *(undefined4 *)(**(int **)(*(char **)(panel->icon_part[0] + 0x18) + 0x20) + 4) =
         *(undefined4 *)((char *)&g_game_659b18 + icon_index * 4);
    *(undefined4 *)(**(int **)(*(char **)(panel->icon_part[1] + 0x18) + 0x20) + 4) =
         *(undefined4 *)((char *)&g_game_659b18 + panel->icon_index * 4);
    *(undefined4 *)(**(int **)(*(char **)(panel->icon_part[2] + 0x18) + 0x20) + 4) =
         *(undefined4 *)((char *)&g_game_659b18 + panel->icon_index * 4);
    *(undefined4 *)(**(int **)(*(char **)(panel->icon_part[3] + 0x18) + 0x20) + 4) =
         *(undefined4 *)((char *)&g_game_659b18 + panel->icon_index * 4);
    *(undefined4 *)(**(int **)(*(char **)(panel->icon_part[4] + 0x18) + 0x20) + 4) =
         *(undefined4 *)((char *)&g_game_659b18 + panel->icon_index * 4);
    *(undefined4 *)(**(int **)(*(char **)(panel->icon_part[5] + 0x18) + 0x20) + 4) =
         *(undefined4 *)((char *)&g_game_659b18 + panel->icon_index * 4);
    *(undefined4 *)(**(int **)(*(char **)(panel->icon_part[6] + 0x18) + 0x20) + 4) =
         *(undefined4 *)((char *)&g_game_659b18 + panel->icon_index * 4);
    *(undefined4 *)(**(int **)(*(char **)(panel->icon_part[7] + 0x18) + 0x20) + 4) =
         *(undefined4 *)((char *)&g_game_659b18 + panel->icon_index * 4);
    *(undefined4 *)(**(int **)(*(char **)(panel->icon_part[8] + 0x18) + 0x20) + 4) =
         *(undefined4 *)((char *)&g_game_659b18 + panel->icon_index * 4);
    *(undefined4 *)(**(int **)(*(char **)(panel->icon_part[9] + 0x18) + 0x20) + 4) =
         *(undefined4 *)((char *)&g_game_659b18 + panel->icon_index * 4);
    *(undefined4 *)(**(int **)(*(char **)(panel->icon_part_178 + 0x18) + 0x20) + 4) =
         *(undefined4 *)((char *)&g_game_659b18 + panel->icon_index * 4);
    *(undefined4 *)(**(int **)(*(char **)(panel->icon_part_174 + 0x18) + 0x20) + 4) =
         *(undefined4 *)((char *)&g_game_659b18 + panel->icon_index * 4);
  }
  return;
}

/* hudpanel_load_indicator_update */

/* "places the load indicator for phase 7 (0.35,0.2, scale 1.5) or otherwise (-0.375,-0.3, 1.125)"
    */

void hudpanel_load_indicator_update(void)

{
  int race_phase;
  struct {
    RwV3d scale;
    RwV3d pos_default;
    RwV3d pos_phase7;
    RwMatrix matrix;
  } frame;
  
  frame.matrix.flags = frame.matrix.flags | 0x20003;
  frame.pos_default.x = -0.375;
  frame.pos_default.y = -0.3;
  frame.pos_default.z = 1.0;
  frame.pos_phase7.x = 0.35;
  frame.pos_phase7.y = 0.2;
  frame.pos_phase7.z = 1.0;
  frame.scale.x = 1.0;
  frame.scale.y = 1.0;
  frame.scale.z = 1.0;
  g_game_659cfc = g_game_659cf8;
  frame.matrix.at.z = 1.0;
  frame.matrix.up.y = 1.0;
  frame.matrix.right.x = 1.0;
  frame.matrix.up.x = 0.0;
  frame.matrix.right.z = 0.0;
  frame.matrix.right.y = 0.0;
  frame.matrix.at.y = 0.0;
  frame.matrix.at.x = 0.0;
  frame.matrix.up.z = 0.0;
  frame.matrix.pos.z = 0.0;
  frame.matrix.pos.y = 0.0;
  frame.matrix.pos.x = 0.0;
  race_phase = race_get_phase();
  if (race_phase == 7) {
    RwMatrixTranslate(&frame.matrix,&frame.pos_phase7,rwCOMBINEREPLACE);
    frame.scale.x = frame.scale.x * k_f_1_5;
    frame.scale.y = frame.scale.y * k_f_1_5;
    frame.scale.z = frame.scale.z * k_f_1_5;
  }
  else {
    RwMatrixTranslate(&frame.matrix,&frame.pos_default,rwCOMBINEREPLACE);
    frame.scale.x = frame.scale.x * k_f_1_125;
    frame.scale.y = frame.scale.y * k_f_1_125;
    frame.scale.z = frame.scale.z * k_f_1_125;
  }
  RwFrameTransform(g_load_659ce0,&frame.matrix,rwCOMBINEREPLACE);
  RwFrameScale(g_load_659ce0,&frame.scale,rwCOMBINEPRECONCAT);
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
