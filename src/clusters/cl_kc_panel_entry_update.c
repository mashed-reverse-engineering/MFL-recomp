// cluster-entries: kc_panel_entry_update
/* Installed cluster kc_panel_entry_update (tools/cluster_install.py from output/cluster_pilot_h2/kc_panel_entry_update.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*0040c9b0*/ extern int player_get_points_change_visual();
/*0040cc60*/ extern int player_get_points();
/*0040ce30*/ extern int score_target_a();
/*004d1500*/ extern struct RwFrame * RwFrameScale(struct RwFrame *p0, struct RwV3d *p1, enum RwOpCombineType p2);
/*004d15a0*/ extern struct RwFrame * RwFrameTransform(struct RwFrame *p0, RwMatrix *p1, enum RwOpCombineType p2);
/*004d4870*/ extern RwMatrix * RwMatrixTranslate(RwMatrix *p0, struct RwV3d *p1, enum RwOpCombineType p2);
/*005dd330*/ extern float float_1_0;
/*005dd374*/ extern float k_f_0_2;
/*005dd9c4*/ extern float k_f_0_15;
/*005dda20*/ extern float k_f_4_2949673e9;
/*005de288*/ extern float k_f_1_125;
/*00603f4c*/ extern undefined4 g_hudpanel_603f4c;
/*006592e0*/ extern undefined g_kc_panel_6592e0;
/*006592f4*/ extern undefined g_kc_panel_6592f4;
/*00659308*/ extern undefined g_kc_panel_659308;
/*00659318*/ extern undefined4 g_kc_panel_659318;
/*00659328*/ extern undefined g_kc_panel_659328;
/*00659790*/ extern undefined4 g_kc_659790;
extern double sin(double);
/* kc_panel_entry_update */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* WARNING: Unknown calling convention */
/*  */

/* entry fields used: +0 .. +0x7c visibility of the 0x20 parts (written at the end of the part loop), +0x80 part present table, +0x108 player_or_part index, +0x10c state flag bits (0x40..0x200 from the g_kc_panel_* tables, 0x800 / 0x400 icon variant, 0x1000..0x10000 points change) */
void kc_panel_entry_update_n(int entry)

{
  uint flags;
  undefined4 points_change;
  int slot_index;
  int player_or_part;
  float10 pulse;
  struct {
    RwV3d scale;
    RwMatrix mtx;
  } frame;
  
  player_or_part = *(int *)(entry + 0x108);
  frame.scale.x = 1.0;
  frame.scale.y = 1.0;
  frame.scale.z = 1.0;
  if (*(int *)((char *)&g_kc_panel_659308 + player_or_part * 4) == 0) {
    flags = *(uint *)(entry + 0x10c) & 0xffffffbf;
  }
  else {
    flags = *(uint *)(entry + 0x10c) | 0x40;
  }
  *(uint *)(entry + 0x10c) = flags;
  if (*(int *)((char *)&g_kc_panel_659328 + player_or_part * 4) == 0) {
    flags = *(uint *)(entry + 0x10c) & 0xffffff7f;
  }
  else {
    flags = *(uint *)(entry + 0x10c) | 0x80;
  }
  *(uint *)(entry + 0x10c) = flags;
  if (*(int *)((char *)&g_kc_panel_6592f4 + player_or_part * 4) == 0) {
    flags = *(uint *)(entry + 0x10c) & 0xfffffeff;
  }
  else {
    flags = *(uint *)(entry + 0x10c) | 0x100;
  }
  *(uint *)(entry + 0x10c) = flags;
  if (*(int *)((char *)&g_kc_panel_6592e0 + player_or_part * 4) == 0) {
    flags = *(uint *)(entry + 0x10c) & 0xfffffdff;
  }
  else {
    flags = *(uint *)(entry + 0x10c) | 0x200;
  }
  *(uint *)(entry + 0x10c) = flags;
  if (((char)flags < '\0') && ((flags & 0x100) != 0)) {
    *(uint *)(entry + 0x10c) = flags & 0xfffffe7f;
  }
  flags = *(uint *)(entry + 0x10c);
  if (((flags & 0x40) != 0) && ((flags & 0x100) != 0)) {
    *(uint *)(entry + 0x10c) = flags & 0xfffffeff;
  }
  points_change = player_get_points_change_visual(player_or_part);
  flags = *(uint *)(entry + 0x10c) & 0xfffe0fff;
  *(uint *)(entry + 0x10c) = flags;
  switch(points_change) {
  default:
    flags = flags | 0x4000;
    break;
  case 1:
    flags = flags | 0x2000;
    break;
  case 2:
    flags = flags | 0x1000;
    break;
  case 0xfffffffe:
    flags = flags | 0x10000;
    break;
  case 0xffffffff:
    flags = flags | 0x8000;
  }
  *(uint *)(entry + 0x10c) = flags;
  player_or_part = 0;
  do {
    flags = 0;
    if (*(int *)(entry + 0x80 + player_or_part * 4) != 0) {
      switch(player_or_part) {
      case 1:
      case 2:
        flags = *(uint *)(entry + 0x10c) & 0x800;
        break;
      case 3:
        if (((*(uint *)(entry + 0x10c) & 0x40) == 0) ||
           ((*(uint *)(entry + 0x10c) & 0x800) == 0)) {
LAB_0041f76a:
          flags = 0;
        }
        else {
          flags = 1;
        }
        break;
      case 4:
      case 5:
        flags = *(uint *)(entry + 0x10c) & 0x400;
        break;
      case 6:
        if (((*(uint *)(entry + 0x10c) & 0x40) == 0) ||
           ((*(uint *)(entry + 0x10c) & 0x400) == 0)) goto LAB_0041f76a;
        flags = 1;
        break;
      case 7:
        flags = *(uint *)(entry + 0x10c) & 1;
        break;
      case 8:
        flags = *(uint *)(entry + 0x10c) & 2;
        break;
      case 9:
        flags = *(uint *)(entry + 0x10c) & 4;
        break;
      case 10:
        flags = *(uint *)(entry + 0x10c) & 8;
        break;
      case 0xb:
        flags = *(uint *)(entry + 0x10c) & 0x10;
        break;
      case 0xc:
        flags = *(uint *)(entry + 0x10c) & 0x20;
        break;
      case 0xf:
        if (((*(uint *)(entry + 0x10c) & 0x4000) == 0) ||
           ((*(uint *)(entry + 0x10c) & 0x800) == 0)) goto LAB_0041f76a;
        flags = 1;
        break;
      case 0x10:
        if (((*(uint *)(entry + 0x10c) & 0x2000) == 0) ||
           ((*(uint *)(entry + 0x10c) & 0x800) == 0)) goto LAB_0041f76a;
        flags = 1;
        break;
      case 0x11:
        if (((*(uint *)(entry + 0x10c) & 0x1000) == 0) ||
           ((*(uint *)(entry + 0x10c) & 0x800) == 0)) goto LAB_0041f76a;
        flags = 1;
        break;
      case 0x12:
        if ((-1 < (char)(*(uint *)(entry + 0x10c) >> 8)) ||
           ((*(uint *)(entry + 0x10c) & 0x800) == 0)) goto LAB_0041f76a;
        flags = 1;
        break;
      case 0x13:
        if (((*(uint *)(entry + 0x10c) & 0x10000) == 0) ||
           ((*(uint *)(entry + 0x10c) & 0x800) == 0)) goto LAB_0041f76a;
        flags = 1;
        break;
      case 0x14:
        if (((*(uint *)(entry + 0x10c) & 0x4000) == 0) ||
           ((*(uint *)(entry + 0x10c) & 0x400) == 0)) goto LAB_0041f76a;
        flags = 1;
        break;
      case 0x15:
        if (((*(uint *)(entry + 0x10c) & 0x2000) == 0) ||
           ((*(uint *)(entry + 0x10c) & 0x400) == 0)) goto LAB_0041f76a;
        flags = 1;
        break;
      case 0x16:
        if (((*(uint *)(entry + 0x10c) & 0x1000) == 0) ||
           ((*(uint *)(entry + 0x10c) & 0x400) == 0)) goto LAB_0041f76a;
        flags = 1;
        break;
      case 0x17:
        if ((-1 < (char)(*(uint *)(entry + 0x10c) >> 8)) ||
           ((*(uint *)(entry + 0x10c) & 0x400) == 0)) goto LAB_0041f76a;
        flags = 1;
        break;
      case 0x18:
        if (((*(uint *)(entry + 0x10c) & 0x10000) == 0) ||
           ((*(uint *)(entry + 0x10c) & 0x400) == 0)) goto LAB_0041f76a;
      case 0:
      case 0x19:
        flags = 1;
      }
      *(uint *)(entry + player_or_part * 4) = flags;
    }
    player_or_part = player_or_part + 1;
  } while (player_or_part < 0x20);
  player_or_part = 0;
  do {
    slot_index = player_or_part;
    if (*(int *)(entry + 0x108) == (&g_kc_panel_659318)[player_or_part]) break;
    player_or_part = player_or_part + 1;
    slot_index = 0;
  } while (player_or_part < 4);
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
  RwMatrixTranslate(&frame.mtx,(RwV3d *)((char *)&g_hudpanel_603f4c + slot_index * 12),rwCOMBINEREPLACE);
  RwFrameTransform(*(RwFrame **)(entry + 0x104),&frame.mtx,rwCOMBINEREPLACE);
  frame.scale.x = frame.scale.x * k_f_1_125;
  frame.scale.y = frame.scale.y * k_f_1_125;
  frame.scale.z = frame.scale.z * k_f_1_125;
  RwFrameScale(*(RwFrame **)(entry + 0x104),&frame.scale,rwCOMBINEPRECONCAT);
  player_or_part = player_get_points(*(undefined4 *)(entry + 0x108));
  slot_index = score_target_a();
  frame.scale.x = (float)player_or_part / (float)slot_index;
  frame.scale.z = 1.0;
  frame.scale.y = 1.0;
  RwFrameScale(*(RwFrame **)(*(int *)(entry + 0x94) + 4),&frame.scale,rwCOMBINEREPLACE);
  RwFrameScale(*(RwFrame **)(*(int *)(entry + 0x88) + 4),&frame.scale,rwCOMBINEREPLACE);
  if ((*(byte *)(entry + 0x10c) & 0x40) == 0) {
    frame.scale.y = 1.0;
    frame.scale.x = 1.0;
  }
  else {
    pulse = (float10)(int)g_kc_659790;
    if ((int)g_kc_659790 < 0) {
      pulse = pulse + (float10)k_f_4_2949673e9;
    }
    pulse = (float10)sin(pulse * (float10)k_f_0_2);
    pulse = pulse * (float10)k_f_0_15 + (float10)float_1_0;
    frame.scale.y = (RwReal)pulse;
    frame.scale.x = (RwReal)pulse;
  }
  frame.scale.z = 1.0;
  RwFrameScale(*(RwFrame **)(*(int *)(entry + 0x98) + 4),&frame.scale,rwCOMBINEREPLACE);
  RwFrameScale(*(RwFrame **)(*(int *)(entry + 0x8c) + 4),&frame.scale,rwCOMBINEREPLACE);
  return;
}

/* entry thunk at the original address of kc_panel_entry_update: register inputs (esi) -> natural C kc_panel_entry_update_n; ret 0 */
__declspec(naked) void kc_panel_entry_update(void)
{
    __asm {
        push esi
        call kc_panel_entry_update_n
        add esp, 4
        ret
    }
}
