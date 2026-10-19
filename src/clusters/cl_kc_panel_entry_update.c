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

void kc_panel_entry_update_n(int in_esi)

{
  uint uVar1;
  undefined4 uVar2;
  int iVar3;
  int points;
  float10 fVar4;
  struct {
    RwV3d local_4c;
    RwMatrix local_40;
  } frame;
  
  points = *(int *)(in_esi + 0x108);
  frame.local_4c.x = 1.0;
  frame.local_4c.y = 1.0;
  frame.local_4c.z = 1.0;
  if (*(int *)((char *)&g_kc_panel_659308 + points * 4) == 0) {
    uVar1 = *(uint *)(in_esi + 0x10c) & 0xffffffbf;
  }
  else {
    uVar1 = *(uint *)(in_esi + 0x10c) | 0x40;
  }
  *(uint *)(in_esi + 0x10c) = uVar1;
  if (*(int *)((char *)&g_kc_panel_659328 + points * 4) == 0) {
    uVar1 = *(uint *)(in_esi + 0x10c) & 0xffffff7f;
  }
  else {
    uVar1 = *(uint *)(in_esi + 0x10c) | 0x80;
  }
  *(uint *)(in_esi + 0x10c) = uVar1;
  if (*(int *)((char *)&g_kc_panel_6592f4 + points * 4) == 0) {
    uVar1 = *(uint *)(in_esi + 0x10c) & 0xfffffeff;
  }
  else {
    uVar1 = *(uint *)(in_esi + 0x10c) | 0x100;
  }
  *(uint *)(in_esi + 0x10c) = uVar1;
  if (*(int *)((char *)&g_kc_panel_6592e0 + points * 4) == 0) {
    uVar1 = *(uint *)(in_esi + 0x10c) & 0xfffffdff;
  }
  else {
    uVar1 = *(uint *)(in_esi + 0x10c) | 0x200;
  }
  *(uint *)(in_esi + 0x10c) = uVar1;
  if (((char)uVar1 < '\0') && ((uVar1 & 0x100) != 0)) {
    *(uint *)(in_esi + 0x10c) = uVar1 & 0xfffffe7f;
  }
  uVar1 = *(uint *)(in_esi + 0x10c);
  if (((uVar1 & 0x40) != 0) && ((uVar1 & 0x100) != 0)) {
    *(uint *)(in_esi + 0x10c) = uVar1 & 0xfffffeff;
  }
  uVar2 = player_get_points_change_visual(points);
  uVar1 = *(uint *)(in_esi + 0x10c) & 0xfffe0fff;
  *(uint *)(in_esi + 0x10c) = uVar1;
  switch(uVar2) {
  default:
    uVar1 = uVar1 | 0x4000;
    break;
  case 1:
    uVar1 = uVar1 | 0x2000;
    break;
  case 2:
    uVar1 = uVar1 | 0x1000;
    break;
  case 0xfffffffe:
    uVar1 = uVar1 | 0x10000;
    break;
  case 0xffffffff:
    uVar1 = uVar1 | 0x8000;
  }
  *(uint *)(in_esi + 0x10c) = uVar1;
  points = 0;
  do {
    uVar1 = 0;
    if (*(int *)(in_esi + 0x80 + points * 4) != 0) {
      switch(points) {
      case 1:
      case 2:
        uVar1 = *(uint *)(in_esi + 0x10c) & 0x800;
        break;
      case 3:
        if (((*(uint *)(in_esi + 0x10c) & 0x40) == 0) ||
           ((*(uint *)(in_esi + 0x10c) & 0x800) == 0)) {
LAB_0041f76a:
          uVar1 = 0;
        }
        else {
          uVar1 = 1;
        }
        break;
      case 4:
      case 5:
        uVar1 = *(uint *)(in_esi + 0x10c) & 0x400;
        break;
      case 6:
        if (((*(uint *)(in_esi + 0x10c) & 0x40) == 0) ||
           ((*(uint *)(in_esi + 0x10c) & 0x400) == 0)) goto LAB_0041f76a;
        uVar1 = 1;
        break;
      case 7:
        uVar1 = *(uint *)(in_esi + 0x10c) & 1;
        break;
      case 8:
        uVar1 = *(uint *)(in_esi + 0x10c) & 2;
        break;
      case 9:
        uVar1 = *(uint *)(in_esi + 0x10c) & 4;
        break;
      case 10:
        uVar1 = *(uint *)(in_esi + 0x10c) & 8;
        break;
      case 0xb:
        uVar1 = *(uint *)(in_esi + 0x10c) & 0x10;
        break;
      case 0xc:
        uVar1 = *(uint *)(in_esi + 0x10c) & 0x20;
        break;
      case 0xf:
        if (((*(uint *)(in_esi + 0x10c) & 0x4000) == 0) ||
           ((*(uint *)(in_esi + 0x10c) & 0x800) == 0)) goto LAB_0041f76a;
        uVar1 = 1;
        break;
      case 0x10:
        if (((*(uint *)(in_esi + 0x10c) & 0x2000) == 0) ||
           ((*(uint *)(in_esi + 0x10c) & 0x800) == 0)) goto LAB_0041f76a;
        uVar1 = 1;
        break;
      case 0x11:
        if (((*(uint *)(in_esi + 0x10c) & 0x1000) == 0) ||
           ((*(uint *)(in_esi + 0x10c) & 0x800) == 0)) goto LAB_0041f76a;
        uVar1 = 1;
        break;
      case 0x12:
        if ((-1 < (char)(*(uint *)(in_esi + 0x10c) >> 8)) ||
           ((*(uint *)(in_esi + 0x10c) & 0x800) == 0)) goto LAB_0041f76a;
        uVar1 = 1;
        break;
      case 0x13:
        if (((*(uint *)(in_esi + 0x10c) & 0x10000) == 0) ||
           ((*(uint *)(in_esi + 0x10c) & 0x800) == 0)) goto LAB_0041f76a;
        uVar1 = 1;
        break;
      case 0x14:
        if (((*(uint *)(in_esi + 0x10c) & 0x4000) == 0) ||
           ((*(uint *)(in_esi + 0x10c) & 0x400) == 0)) goto LAB_0041f76a;
        uVar1 = 1;
        break;
      case 0x15:
        if (((*(uint *)(in_esi + 0x10c) & 0x2000) == 0) ||
           ((*(uint *)(in_esi + 0x10c) & 0x400) == 0)) goto LAB_0041f76a;
        uVar1 = 1;
        break;
      case 0x16:
        if (((*(uint *)(in_esi + 0x10c) & 0x1000) == 0) ||
           ((*(uint *)(in_esi + 0x10c) & 0x400) == 0)) goto LAB_0041f76a;
        uVar1 = 1;
        break;
      case 0x17:
        if ((-1 < (char)(*(uint *)(in_esi + 0x10c) >> 8)) ||
           ((*(uint *)(in_esi + 0x10c) & 0x400) == 0)) goto LAB_0041f76a;
        uVar1 = 1;
        break;
      case 0x18:
        if (((*(uint *)(in_esi + 0x10c) & 0x10000) == 0) ||
           ((*(uint *)(in_esi + 0x10c) & 0x400) == 0)) goto LAB_0041f76a;
      case 0:
      case 0x19:
        uVar1 = 1;
      }
      *(uint *)(in_esi + points * 4) = uVar1;
    }
    points = points + 1;
  } while (points < 0x20);
  points = 0;
  do {
    iVar3 = points;
    if (*(int *)(in_esi + 0x108) == (&g_kc_panel_659318)[points]) break;
    points = points + 1;
    iVar3 = 0;
  } while (points < 4);
  frame.local_40.flags = frame.local_40.flags | 0x20003;
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
  RwMatrixTranslate(&frame.local_40,(RwV3d *)((char *)&g_hudpanel_603f4c + iVar3 * 12),rwCOMBINEREPLACE);
  RwFrameTransform(*(RwFrame **)(in_esi + 0x104),&frame.local_40,rwCOMBINEREPLACE);
  frame.local_4c.x = frame.local_4c.x * k_f_1_125;
  frame.local_4c.y = frame.local_4c.y * k_f_1_125;
  frame.local_4c.z = frame.local_4c.z * k_f_1_125;
  RwFrameScale(*(RwFrame **)(in_esi + 0x104),&frame.local_4c,rwCOMBINEPRECONCAT);
  points = player_get_points(*(undefined4 *)(in_esi + 0x108));
  iVar3 = score_target_a();
  frame.local_4c.x = (float)points / (float)iVar3;
  frame.local_4c.z = 1.0;
  frame.local_4c.y = 1.0;
  RwFrameScale(*(RwFrame **)(*(int *)(in_esi + 0x94) + 4),&frame.local_4c,rwCOMBINEREPLACE);
  RwFrameScale(*(RwFrame **)(*(int *)(in_esi + 0x88) + 4),&frame.local_4c,rwCOMBINEREPLACE);
  if ((*(byte *)(in_esi + 0x10c) & 0x40) == 0) {
    frame.local_4c.y = 1.0;
    frame.local_4c.x = 1.0;
  }
  else {
    fVar4 = (float10)(int)g_kc_659790;
    if ((int)g_kc_659790 < 0) {
      fVar4 = fVar4 + (float10)k_f_4_2949673e9;
    }
    fVar4 = (float10)sin(fVar4 * (float10)k_f_0_2);
    fVar4 = fVar4 * (float10)k_f_0_15 + (float10)float_1_0;
    frame.local_4c.y = (RwReal)fVar4;
    frame.local_4c.x = (RwReal)fVar4;
  }
  frame.local_4c.z = 1.0;
  RwFrameScale(*(RwFrame **)(*(int *)(in_esi + 0x98) + 4),&frame.local_4c,rwCOMBINEREPLACE);
  RwFrameScale(*(RwFrame **)(*(int *)(in_esi + 0x8c) + 4),&frame.local_4c,rwCOMBINEREPLACE);
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
