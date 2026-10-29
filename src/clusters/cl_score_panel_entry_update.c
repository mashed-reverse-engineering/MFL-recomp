// cluster-entries: score_panel_entry_update
/* Installed cluster score_panel_entry_update (tools/cluster_install.py from output/cluster_pilot9/score_panel_entry_update.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*0040c9b0*/ extern int player_get_points_change_visual();
/*0040cc60*/ extern int player_get_points();
/*0040ce30*/ extern int score_target_a();
/*004d1500*/ extern struct RwFrame * RwFrameScale(struct RwFrame *p0, struct RwV3d *p1, enum RwOpCombineType p2);
/*004d15a0*/ extern struct RwFrame * RwFrameTransform(struct RwFrame *p0, RwMatrix *p1, enum RwOpCombineType p2);
/*004d4870*/ extern RwMatrix * RwMatrixTranslate(RwMatrix *p0, struct RwV3d *p1, enum RwOpCombineType p2);
/*005dd330*/ extern float float_1_0;
/*005dd374*/ extern float k_f_0_2;
/*005dda20*/ extern float k_f_4_2949673e9;
/*005ddb38*/ extern float k_f_25;
/*005e64a4*/ extern float k_f_0;
/*005de288*/ extern float k_f_1_125;
/*005de28c*/ extern float k_f_0_015;
/*005de2fc*/ extern float k_f_0_04;
/*00603ed4*/ extern undefined g_score_panel_603ed4;
/*00658d98*/ extern undefined g_score_658d98;
/*00658dac*/ extern undefined4 g_score_658dac;
/*00658dc0*/ extern undefined4 g_score_panel_658dc0;
/*00658dc4*/ extern undefined g_score_panel_658dc4;
/*00658dd4*/ extern undefined4 g_score_panel_658dd4;
/*00658de8*/ extern undefined4 g_score_panel_658de8;
/*00658fc0*/ extern undefined g_score_panel_658fc0;
/*00658fd0*/ extern undefined4 g_score_panel_658fd0;
extern double fabs(double);
extern double sin(double);
/* score_panel_entry_update */

/* "player_get_points vs score_target_a: scale / show the icons" */

static int score_panel_entry_scale_parts_r(int p0, RwV3d * p1);
extern void score_panel_entry_scale_parts(void);
static int score_panel_entry_scale_parts_low_r(int p0, RwV3d * p1);
extern void score_panel_entry_scale_parts_low(void);
typedef struct ScorePanelEntry {
    char unknown_00[0x8];
    int field_08; /* 0x8 */
    char unknown_0c[0x8];
    int field_14; /* 0x14 */
    char unknown_18[0x38];
    RwV3d translation; /* 0x50 */
    char unknown_5c[0x4];
    RwFrame *frame; /* 0x60 */
    int player; /* 0x64 */
    uint flags; /* 0x68 */
    char unknown_6c[0x4];
    float anim_counter; /* 0x70 */
} ScorePanelEntry;

/* entry fields used: +0x50 translation (RwV3d), +0x60 RwFrame, +100 player index, +0x68 state flag bits (0x40..0x400 set from the g_score_panel_* tables, 0x800 / 0x1000 icon variant, 0x2000..0x8000 points change), +0x70 animation counter */
void score_panel_entry_update_n(ScorePanelEntry *entry)

{
  RwV3d *translation;
  float tmp_f;
  int points;
  int score_target;
  uint flags;
  int player_or_tmp;
  float10 pulse;
  struct {
    RwV3d scale;
    RwMatrix mtx;
  } frame;
  
  frame.scale.x = 1.0;
  frame.scale.y = 1.0;
  frame.scale.z = 1.0;
  points = player_get_points(entry->player);
  score_target = score_target_a();
  player_or_tmp = entry->player;
  if (*(int *)((char *)&g_score_panel_658dc4 + player_or_tmp * 4) == 0) {
    flags = entry->flags & 0xffffffbf;
  }
  else {
    flags = entry->flags | 0x40;
  }
  entry->flags = flags;
  if (((&g_score_panel_658dd4)[player_or_tmp] == 0) || (g_score_panel_658de8 != 0)) {
    flags = entry->flags & 0xffffff7f;
  }
  else {
    flags = entry->flags | 0x80;
  }
  entry->flags = flags;
  if (((&g_score_658dac)[player_or_tmp] == 0) || (g_score_panel_658dc0 != 0)) {
    flags = entry->flags & 0xfffffeff;
  }
  else {
    flags = entry->flags | 0x100;
  }
  entry->flags = flags;
  if (*(int *)((char *)&g_score_658d98 + player_or_tmp * 4) == 0) {
    flags = entry->flags & 0xfffffdff;
  }
  else {
    flags = entry->flags | 0x200;
  }
  entry->flags = flags;
  if (*(int *)((char *)&g_score_panel_658fc0 + player_or_tmp * 4) == 0) {
    flags = entry->flags & 0xfffffbff;
  }
  else {
    flags = entry->flags | 0x400;
  }
  entry->flags = flags;
  if (((flags & 0x40) != 0) && ((flags & 0x100) != 0)) {
    entry->flags = flags & 0xfffffeff;
  }
  if ((entry->flags & 0x400) != 0) {
    entry->flags = entry->flags & 0xfffffc3f;
  }
  flags = entry->flags;
  entry->flags = flags & 0xffff1fff;
  if ((flags & 0x400) != 0) {
    player_or_tmp = player_get_points_change_visual(player_or_tmp);
    if (player_or_tmp == -2) {
      flags = entry->flags | 0x8000;
LAB_0041e119:
      entry->flags = flags;
    }
    else {
      if (player_or_tmp == -1) {
        flags = entry->flags | 0x4000;
        goto LAB_0041e119;
      }
      if (player_or_tmp == 0) {
        flags = entry->flags | 0x2000;
        goto LAB_0041e119;
      }
    }
    entry->anim_counter = entry->anim_counter + float_1_0;
  }
  if (score_target == 8) {
    entry->flags = entry->flags & 0xffffefff | 0x800;
  }
  else if (score_target == 0xc) {
    entry->flags = entry->flags & 0xfffff7ff | 0x1000;
  }
  translation = &entry->translation;
  if (fabs(*(float *)((char *)&g_score_panel_603ed4 + *(uint *)&entry->player * 0xc)) <
      fabs(entry->translation.x)) {
    if ((*(uint *)&entry->player & 1) == 0) {
      tmp_f = translation->x - k_f_0_015;
    }
    else {
      tmp_f = translation->x + k_f_0_015;
    }
    translation->x = tmp_f;
  }
  frame.mtx.flags = frame.mtx.flags | 0x20003;
  frame.mtx.at.z = 1.0;
  frame.mtx.up.y = 1.0;
  frame.mtx.right.x = 1.0;
  frame.mtx.up.x = 0.0;
  frame.mtx.right.z = 0.0;
  frame.mtx.right.y = 0.0;
  frame.mtx.at.y = 0.0;
  frame.mtx.at.x = 0.0;
  frame.mtx.up.z = 0.0;
  frame.mtx.pos.z = 0.0;
  frame.mtx.pos.y = 0.0;
  frame.mtx.pos.x = 0.0;
  RwMatrixTranslate(&frame.mtx,translation,rwCOMBINEREPLACE);
  RwFrameTransform(entry->frame,&frame.mtx,rwCOMBINEREPLACE);
  frame.scale.x = frame.scale.x * k_f_1_125;
  frame.scale.y = frame.scale.y * k_f_1_125;
  frame.scale.z = frame.scale.z * k_f_1_125;
  RwFrameScale(entry->frame,&frame.scale,rwCOMBINEPRECONCAT);
  frame.scale.x = (float)points / (float)score_target;
  frame.scale.z = 1.0;
  frame.scale.y = 1.0;
  if ((entry->flags & 0x800) == 0) {
    if ((entry->flags & 0x1000) == 0) goto LAB_0041e29b;
    player_or_tmp = entry->field_08;
  }
  else {
    player_or_tmp = entry->field_14;
  }
  RwFrameScale(*(RwFrame **)(player_or_tmp + 4),&frame.scale,rwCOMBINEREPLACE);
LAB_0041e29b:
  if (((char)entry->flags < '\0') && ((entry->flags & 0x40) == 0)) {
    pulse = (float10)g_score_panel_658fd0;
    if ((int)g_score_panel_658fd0 < 0) {
      pulse = pulse + (float10)k_f_4_2949673e9;
    }
    pulse = (float10)sin(pulse * (float10)k_f_0_2);
    pulse = pulse * (float10)k_f_0_2 + (float10)float_1_0;
    frame.scale.y = (RwReal)pulse;
    frame.scale.x = (RwReal)pulse;
  }
  else {
    frame.scale.y = 1.0;
    frame.scale.x = 1.0;
  }
  frame.scale.z = 1.0;
  score_panel_entry_scale_parts_low_r((int)entry,&frame.scale);
  if ((entry->flags & 0x400) != 0) {
    frame.scale.x = float_1_0 - entry->anim_counter * k_f_0_04;
    if (frame.scale.x <= k_f_0) {
      frame.scale.x = k_f_0;
    }
    frame.scale.z = 1.0;
    frame.scale.y = frame.scale.x;
    score_panel_entry_scale_parts_low_r((int)entry,&frame.scale);
    tmp_f = (entry->anim_counter - k_f_25) * k_f_0_04;
    if (((tmp_f <= k_f_0) || (frame.scale.x = float_1_0, !(float_1_0 <= tmp_f)))
       && (frame.scale.x = tmp_f, tmp_f <= k_f_0)) {
      frame.scale.x = k_f_0;
    }
    frame.scale.z = 1.0;
    frame.scale.y = frame.scale.x;
    score_panel_entry_scale_parts_r((int)entry,&frame.scale);
  }
  return;
}

/* score_panel_entry_scale_parts_r: C -> original score_panel_entry_scale_parts (registers esi,edi) */
__declspec(naked) int score_panel_entry_scale_parts_r(int p0, RwV3d * p1)
{
    __asm {
        push esi
        push edi
        mov esi, dword ptr [esp + 12]
        mov edi, dword ptr [esp + 16]
        call score_panel_entry_scale_parts
        pop edi
        pop esi
        ret
    }
}

/* score_panel_entry_scale_parts_low_r: C -> original score_panel_entry_scale_parts_low (registers esi,edi) */
__declspec(naked) int score_panel_entry_scale_parts_low_r(int p0, RwV3d * p1)
{
    __asm {
        push esi
        push edi
        mov esi, dword ptr [esp + 12]
        mov edi, dword ptr [esp + 16]
        call score_panel_entry_scale_parts_low
        pop edi
        pop esi
        ret
    }
}

/* entry thunk at the original address of score_panel_entry_update: register inputs (eax) -> natural C score_panel_entry_update_n; ret 0 */
__declspec(naked) void score_panel_entry_update(void)
{
    __asm {
        push eax
        call score_panel_entry_update_n
        add esp, 4
        ret
    }
}
