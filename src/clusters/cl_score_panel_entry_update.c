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
/*005ddc28*/ extern float k_f_0;
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

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* WARNING: Unknown calling convention */
/* "player_get_points vs score_target_a: scale / show the icons" */

static int score_panel_entry_scale_parts_r(int p0, RwV3d * p1);
extern void score_panel_entry_scale_parts(void);
static int score_panel_entry_scale_parts_low_r(int p0, RwV3d * p1);
extern void score_panel_entry_scale_parts_low(void);
void score_panel_entry_update_n(int in_eax)

{
  RwV3d *translation;
  float fVar1;
  int points;
  int iVar2;
  uint uVar3;
  int iVar4;
  float10 fVar5;
  struct {
    RwV3d local_4c;
    RwMatrix local_40;
  } frame;
  
  frame.local_4c.x = 1.0;
  frame.local_4c.y = 1.0;
  frame.local_4c.z = 1.0;
  points = player_get_points(*(undefined4 *)(in_eax + 100));
  iVar2 = score_target_a();
  iVar4 = *(int *)(in_eax + 100);
  if (*(int *)((char *)&g_score_panel_658dc4 + iVar4 * 4) == 0) {
    uVar3 = *(uint *)(in_eax + 0x68) & 0xffffffbf;
  }
  else {
    uVar3 = *(uint *)(in_eax + 0x68) | 0x40;
  }
  *(uint *)(in_eax + 0x68) = uVar3;
  if (((&g_score_panel_658dd4)[iVar4] == 0) || (g_score_panel_658de8 != 0)) {
    uVar3 = *(uint *)(in_eax + 0x68) & 0xffffff7f;
  }
  else {
    uVar3 = *(uint *)(in_eax + 0x68) | 0x80;
  }
  *(uint *)(in_eax + 0x68) = uVar3;
  if (((&g_score_658dac)[iVar4] == 0) || (g_score_panel_658dc0 != 0)) {
    uVar3 = *(uint *)(in_eax + 0x68) & 0xfffffeff;
  }
  else {
    uVar3 = *(uint *)(in_eax + 0x68) | 0x100;
  }
  *(uint *)(in_eax + 0x68) = uVar3;
  if (*(int *)((char *)&g_score_658d98 + iVar4 * 4) == 0) {
    uVar3 = *(uint *)(in_eax + 0x68) & 0xfffffdff;
  }
  else {
    uVar3 = *(uint *)(in_eax + 0x68) | 0x200;
  }
  *(uint *)(in_eax + 0x68) = uVar3;
  if (*(int *)((char *)&g_score_panel_658fc0 + iVar4 * 4) == 0) {
    uVar3 = *(uint *)(in_eax + 0x68) & 0xfffffbff;
  }
  else {
    uVar3 = *(uint *)(in_eax + 0x68) | 0x400;
  }
  *(uint *)(in_eax + 0x68) = uVar3;
  if (((uVar3 & 0x40) != 0) && ((uVar3 & 0x100) != 0)) {
    *(uint *)(in_eax + 0x68) = uVar3 & 0xfffffeff;
  }
  if ((*(uint *)(in_eax + 0x68) & 0x400) != 0) {
    *(uint *)(in_eax + 0x68) = *(uint *)(in_eax + 0x68) & 0xfffffc3f;
  }
  uVar3 = *(uint *)(in_eax + 0x68);
  *(uint *)(in_eax + 0x68) = uVar3 & 0xffff1fff;
  if ((uVar3 & 0x400) != 0) {
    iVar4 = player_get_points_change_visual(iVar4);
    if (iVar4 == -2) {
      uVar3 = *(uint *)(in_eax + 0x68) | 0x8000;
LAB_0041e119:
      *(uint *)(in_eax + 0x68) = uVar3;
    }
    else {
      if (iVar4 == -1) {
        uVar3 = *(uint *)(in_eax + 0x68) | 0x4000;
        goto LAB_0041e119;
      }
      if (iVar4 == 0) {
        uVar3 = *(uint *)(in_eax + 0x68) | 0x2000;
        goto LAB_0041e119;
      }
    }
    *(float *)(in_eax + 0x70) = *(float *)(in_eax + 0x70) + float_1_0;
  }
  if (iVar2 == 8) {
    *(uint *)(in_eax + 0x68) = *(uint *)(in_eax + 0x68) & 0xffffefff | 0x800;
  }
  else if (iVar2 == 0xc) {
    *(uint *)(in_eax + 0x68) = *(uint *)(in_eax + 0x68) & 0xfffff7ff | 0x1000;
  }
  translation = (RwV3d *)(in_eax + 0x50);
  if (fabs(*(float *)((char *)&g_score_panel_603ed4 + *(uint *)(in_eax + 100) * 0xc)) <
      fabs(*(float *)(in_eax + 0x50))) {
    if ((*(uint *)(in_eax + 100) & 1) == 0) {
      fVar1 = translation->x - k_f_0_015;
    }
    else {
      fVar1 = translation->x + k_f_0_015;
    }
    translation->x = fVar1;
  }
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
  RwMatrixTranslate(&frame.local_40,translation,rwCOMBINEREPLACE);
  RwFrameTransform(*(RwFrame **)(in_eax + 0x60),&frame.local_40,rwCOMBINEREPLACE);
  frame.local_4c.x = frame.local_4c.x * k_f_1_125;
  frame.local_4c.y = frame.local_4c.y * k_f_1_125;
  frame.local_4c.z = frame.local_4c.z * k_f_1_125;
  RwFrameScale(*(RwFrame **)(in_eax + 0x60),&frame.local_4c,rwCOMBINEPRECONCAT);
  frame.local_4c.x = (float)points / (float)iVar2;
  frame.local_4c.z = 1.0;
  frame.local_4c.y = 1.0;
  if ((*(uint *)(in_eax + 0x68) & 0x800) == 0) {
    if ((*(uint *)(in_eax + 0x68) & 0x1000) == 0) goto LAB_0041e29b;
    iVar4 = *(int *)(in_eax + 8);
  }
  else {
    iVar4 = *(int *)(in_eax + 0x14);
  }
  RwFrameScale(*(RwFrame **)(iVar4 + 4),&frame.local_4c,rwCOMBINEREPLACE);
LAB_0041e29b:
  if (((char)*(uint *)(in_eax + 0x68) < '\0') && ((*(uint *)(in_eax + 0x68) & 0x40) == 0)) {
    fVar5 = (float10)g_score_panel_658fd0;
    if ((int)g_score_panel_658fd0 < 0) {
      fVar5 = fVar5 + (float10)k_f_4_2949673e9;
    }
    fVar5 = (float10)sin(fVar5 * (float10)k_f_0_2);
    fVar5 = fVar5 * (float10)k_f_0_2 + (float10)float_1_0;
    frame.local_4c.y = (RwReal)fVar5;
    frame.local_4c.x = (RwReal)fVar5;
  }
  else {
    frame.local_4c.y = 1.0;
    frame.local_4c.x = 1.0;
  }
  frame.local_4c.z = 1.0;
  score_panel_entry_scale_parts_low_r(in_eax,&frame.local_4c);
  if ((*(uint *)(in_eax + 0x68) & 0x400) != 0) {
    frame.local_4c.x = float_1_0 - *(float *)(in_eax + 0x70) * k_f_0_04;
    if (frame.local_4c.x <= k_f_0) {
      frame.local_4c.x = k_f_0;
    }
    frame.local_4c.z = 1.0;
    frame.local_4c.y = frame.local_4c.x;
    score_panel_entry_scale_parts_low_r(in_eax,&frame.local_4c);
    fVar1 = (*(float *)(in_eax + 0x70) - k_f_25) * k_f_0_04;
    if (((fVar1 <= k_f_0) || (frame.local_4c.x = float_1_0, float_1_0 > fVar1))
       && (frame.local_4c.x = fVar1, fVar1 <= k_f_0)) {
      frame.local_4c.x = k_f_0;
    }
    frame.local_4c.z = 1.0;
    frame.local_4c.y = frame.local_4c.x;
    score_panel_entry_scale_parts_r(in_eax,&frame.local_4c);
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
