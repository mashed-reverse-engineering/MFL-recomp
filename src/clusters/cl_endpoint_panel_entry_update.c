// cluster-entries: endpoint_panel_entry_update
/* Installed cluster endpoint_panel_entry_update (tools/cluster_install.py from output/cluster_pilot_h2/endpoint_panel_entry_update.c); entries are patched at their original addresses by build_overlay.py --clusters. */
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
/*00603f94*/ extern undefined g_endpoint_panel_603f94;
/*00659798*/ extern undefined g_endpoint_panel_659798;
/*006597a8*/ extern undefined4 g_endpoint_panel_6597a8;
/*00659a78*/ extern undefined4 g_endpoint_659a78;
extern double sin(double);
/* endpoint_panel_entry_update */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* WARNING: Unknown calling convention */
/* "endpoint panel entry (0x6597B8, stride 0x160)" */

void endpoint_panel_entry_update_n(int *in_esi)

{
  uint uVar1;
  undefined4 uVar2;
  uint uVar3;
  int points;
  int iVar4;
  bool bVar5;
  float10 fVar6;
  struct {
    RwV3d local_4c;
    RwMatrix local_40;
  } frame;
  
  frame.local_4c.x = 1.0;
  frame.local_4c.y = 1.0;
  frame.local_4c.z = 1.0;
  if (*(int *)((char *)&g_endpoint_panel_659798 + *in_esi * 4) == 0) {
    uVar3 = in_esi[0x57] & 0xfffbffff;
  }
  else {
    uVar3 = in_esi[0x57] | 0x40000;
  }
  in_esi[0x57] = uVar3;
  uVar2 = player_get_points_change_visual(*in_esi);
  uVar3 = in_esi[0x57] & 0x83ffffff;
  in_esi[0x57] = uVar3;
  switch(uVar2) {
  case 0:
    uVar3 = uVar3 | 0x10000000;
    break;
  case 1:
    uVar3 = uVar3 | 0x8000000;
    break;
  case 2:
    uVar3 = uVar3 | 0x4000000;
    break;
  case 0xfffffffe:
    uVar3 = uVar3 | 0x40000000;
    break;
  case 0xffffffff:
    uVar3 = uVar3 | 0x20000000;
    break;
  default:
    goto switchD_0041fee5_default;
  }
  in_esi[0x57] = uVar3;
switchD_0041fee5_default:
  points = 0;
  do {
    if (in_esi[points + 0x2c] == 0) goto LAB_004200c8;
    switch(points) {
    case 0:
    case 1:
    case 2:
    case 3:
      goto switchD_0041ff38_caseD_0;
    case 4:
      uVar3 = in_esi[0x57] & 0x40000;
      goto LAB_004200c4;
    case 5:
      uVar3 = in_esi[0x57];
      uVar1 = uVar3 & 0x3000000;
      goto LAB_0041ff5a;
    case 6:
      if ((in_esi[0x57] & 0x3000000U) == 0) break;
      bVar5 = (in_esi[0x57] & 2U) == 0;
      goto LAB_0041ff62;
    case 7:
      if ((in_esi[0x57] & 0x3000000U) != 0) {
        bVar5 = (in_esi[0x57] & 4U) == 0;
        goto LAB_0041ff62;
      }
      break;
    case 8:
      if ((in_esi[0x57] & 0x3000000U) != 0) {
        bVar5 = (in_esi[0x57] & 8U) == 0;
        goto LAB_0041ff62;
      }
      break;
    case 9:
      if ((in_esi[0x57] & 0x3000000U) != 0) {
        bVar5 = (in_esi[0x57] & 0x10U) == 0;
        goto LAB_0041ff62;
      }
      break;
    case 10:
      if ((in_esi[0x57] & 0x3000000U) != 0) {
        bVar5 = (in_esi[0x57] & 0x20U) == 0;
        goto LAB_0041ff62;
      }
      break;
    case 0xb:
      if ((in_esi[0x57] & 0x3000000U) != 0) {
        bVar5 = (in_esi[0x57] & 0x40U) == 0;
        goto LAB_0041ff62;
      }
      break;
    case 0xc:
      if (((in_esi[0x57] & 0x3000000U) == 0) || (-1 < (char)in_esi[0x57])) break;
      goto switchD_0041ff38_caseD_0;
    case 0xd:
      if ((in_esi[0x57] & 0x3000000U) != 0) {
        bVar5 = (in_esi[0x57] & 0x100U) == 0;
        goto LAB_0041ff62;
      }
      break;
    case 0xe:
      if ((in_esi[0x57] & 0x3000000U) != 0) {
        bVar5 = (in_esi[0x57] & 0x200U) == 0;
        goto LAB_0041ff62;
      }
      break;
    case 0xf:
      if ((in_esi[0x57] & 0x3000000U) != 0) {
        bVar5 = (in_esi[0x57] & 0x400U) == 0;
        goto LAB_0041ff62;
      }
      break;
    case 0x10:
      if ((in_esi[0x57] & 0x3000000U) != 0) {
        bVar5 = (in_esi[0x57] & 0x800U) == 0;
        goto LAB_0041ff62;
      }
      break;
    case 0x11:
      uVar3 = in_esi[0x57] & 0x4000000;
      goto LAB_004200c4;
    case 0x12:
      uVar3 = in_esi[0x57] & 0x8000000;
      goto LAB_004200c4;
    case 0x13:
      uVar3 = in_esi[0x57] & 0x10000000;
      goto LAB_004200c4;
    case 0x14:
      uVar3 = in_esi[0x57] & 0x20000000;
      goto LAB_004200c4;
    case 0x15:
      uVar3 = in_esi[0x57] & 0x40000000;
      goto LAB_004200c4;
    case 0x16:
      if ((in_esi[0x57] & 0x2000000U) != 0) {
        bVar5 = (in_esi[0x57] & 0x1000U) == 0;
        goto LAB_0041ff62;
      }
      break;
    case 0x17:
      if ((in_esi[0x57] & 0x2000000U) != 0) {
        bVar5 = (in_esi[0x57] & 0x2000U) == 0;
        goto LAB_0041ff62;
      }
      break;
    case 0x18:
      if ((in_esi[0x57] & 0x2000000U) != 0) {
        bVar5 = (in_esi[0x57] & 0x4000U) == 0;
        goto LAB_0041ff62;
      }
      break;
    case 0x19:
      if (((in_esi[0x57] & 0x2000000U) == 0) || (-1 < (char)((uint)in_esi[0x57] >> 8))) break;
      uVar3 = 1;
      goto LAB_004200c4;
    case 0x1a:
      if ((in_esi[0x57] & 0x2000000U) != 0) {
        bVar5 = (in_esi[0x57] & 0x10000U) == 0;
        goto LAB_0041ff62;
      }
      break;
    case 0x1b:
      if ((in_esi[0x57] & 0x2000000U) != 0) {
        bVar5 = (in_esi[0x57] & 0x20000U) == 0;
        goto LAB_0041ff62;
      }
      break;
    case 0x1c:
      uVar3 = in_esi[0x57];
      uVar1 = uVar3 & 0x800000;
LAB_0041ff5a:
      if (uVar1 != 0) {
        bVar5 = (uVar3 & 1) == 0;
LAB_0041ff62:
        if (!bVar5) goto switchD_0041ff38_caseD_0;
      }
      break;
    case 0x1d:
      if (((in_esi[0x57] & 0x800000U) == 0) || ((in_esi[0x57] & 2U) == 0)) break;
      uVar3 = 1;
      goto LAB_004200c4;
    case 0x1e:
      if (((in_esi[0x57] & 0x800000U) == 0) || ((in_esi[0x57] & 4U) == 0)) break;
      uVar3 = 1;
      goto LAB_004200c4;
    case 0x1f:
      if (((in_esi[0x57] & 0x800000U) == 0) || ((in_esi[0x57] & 8U) == 0)) break;
      uVar3 = 1;
      goto LAB_004200c4;
    case 0x20:
      if (((in_esi[0x57] & 0x800000U) == 0) || ((in_esi[0x57] & 0x10U) == 0)) break;
      uVar3 = 1;
      goto LAB_004200c4;
    case 0x21:
      if (((in_esi[0x57] & 0x800000U) == 0) || ((in_esi[0x57] & 0x20U) == 0)) break;
      uVar3 = 1;
      goto LAB_004200c4;
    }
    uVar3 = 0;
LAB_004200c4:
    in_esi[points + 4] = uVar3;
LAB_004200c8:
    points = points + 1;
  } while (points < 0x28);
  points = 0;
  iVar4 = 1;
  if (0 < in_esi[3]) {
    do {
      if (in_esi[points] == g_endpoint_panel_6597a8) {
        iVar4 = 0;
        break;
      }
      points = points + 1;
    } while (points < in_esi[3]);
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
  RwMatrixTranslate(&frame.local_40,(RwV3d *)((char *)&g_endpoint_panel_603f94 + iVar4 * 0xc),rwCOMBINEREPLACE);
  RwFrameTransform((RwFrame *)in_esi[0x55],&frame.local_40,rwCOMBINEREPLACE);
  frame.local_4c.x = frame.local_4c.x * k_f_1_125;
  frame.local_4c.y = frame.local_4c.y * k_f_1_125;
  frame.local_4c.z = frame.local_4c.z * k_f_1_125;
  RwFrameScale((RwFrame *)in_esi[0x55],&frame.local_4c,rwCOMBINEPRECONCAT);
  points = player_get_points(*in_esi);
  iVar4 = score_target_a();
  frame.local_4c.x = (float)points / (float)iVar4;
  frame.local_4c.z = 1.0;
  frame.local_4c.y = 1.0;
  RwFrameScale(*(RwFrame **)(in_esi[0x2e] + 4),&frame.local_4c,rwCOMBINEREPLACE);
  if ((in_esi[0x57] & 0x40000U) == 0) {
    frame.local_4c.y = 1.0;
    frame.local_4c.x = 1.0;
  }
  else {
    fVar6 = (float10)(int)g_endpoint_659a78;
    if ((int)g_endpoint_659a78 < 0) {
      fVar6 = fVar6 + (float10)k_f_4_2949673e9;
    }
    fVar6 = (float10)sin(fVar6 * (float10)k_f_0_2);
    fVar6 = fVar6 * (float10)k_f_0_15 + (float10)float_1_0;
    frame.local_4c.y = (RwReal)fVar6;
    frame.local_4c.x = (RwReal)fVar6;
  }
  frame.local_4c.z = 1.0;
  RwFrameScale(*(RwFrame **)(in_esi[0x30] + 4),&frame.local_4c,rwCOMBINEREPLACE);
  return;
switchD_0041ff38_caseD_0:
  uVar3 = 1;
  goto LAB_004200c4;
}

/* entry thunk at the original address of endpoint_panel_entry_update: register inputs (esi) -> natural C endpoint_panel_entry_update_n; ret 0 */
__declspec(naked) void endpoint_panel_entry_update(void)
{
    __asm {
        push esi
        call endpoint_panel_entry_update_n
        add esp, 4
        ret
    }
}
