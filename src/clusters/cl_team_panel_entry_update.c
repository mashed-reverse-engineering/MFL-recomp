// cluster-entries: team_panel_entry_update
/* Installed cluster team_panel_entry_update (tools/cluster_install.py from output/cluster_pilot7/team_panel_entry_update.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*0040cc60*/ extern int player_get_points();
/*0040ce30*/ extern int score_target_a();
/*004d1500*/ extern struct RwFrame * RwFrameScale(struct RwFrame *p0, struct RwV3d *p1, enum RwOpCombineType p2);
/*004d15a0*/ extern struct RwFrame * RwFrameTransform(struct RwFrame *p0, RwMatrix *p1, enum RwOpCombineType p2);
/*004d4870*/ extern RwMatrix * RwMatrixTranslate(RwMatrix *p0, struct RwV3d *p1, enum RwOpCombineType p2);
/*006592b0*/ extern undefined g_team_6592b0;
/* team_panel_entry_update */

/* WARNING: Unknown calling convention */
/*  */

void team_panel_entry_update_n(int *in_esi)

{
  uint uVar1;
  uint uVar2;
  int iVar3;
  int points;
  bool bVar4;
  struct {
    RwV3d local_4c;
    RwMatrix local_40;
  } frame;
  
  frame.local_4c.x = 1.0;
  frame.local_4c.y = 1.0;
  frame.local_4c.z = 1.0;
  if (*(int *)((char *)&g_team_6592b0 + *in_esi * 4) == 0) {
    uVar2 = in_esi[0x5a] & 0xfffbffff;
  }
  else {
    uVar2 = in_esi[0x5a] | 0x40000;
  }
  in_esi[0x5a] = uVar2;
  points = 0;
  do {
    if (in_esi[points + 0x2c] == 0) goto LAB_0041ea18;
    switch(points) {
    case 0:
    case 1:
    case 2:
    case 3:
      goto switchD_0041e888_caseD_0;
    case 4:
      uVar2 = in_esi[0x5a] & 0x40000;
      goto LAB_0041ea14;
    case 5:
      uVar2 = in_esi[0x5a];
      uVar1 = uVar2 & 0x3000000;
      goto LAB_0041e8aa;
    case 6:
      if ((in_esi[0x5a] & 0x3000000U) == 0) break;
      bVar4 = (in_esi[0x5a] & 2U) == 0;
      goto LAB_0041e8b2;
    case 7:
      if ((in_esi[0x5a] & 0x3000000U) != 0) {
        bVar4 = (in_esi[0x5a] & 4U) == 0;
        goto LAB_0041e8b2;
      }
      break;
    case 8:
      if ((in_esi[0x5a] & 0x3000000U) != 0) {
        bVar4 = (in_esi[0x5a] & 8U) == 0;
        goto LAB_0041e8b2;
      }
      break;
    case 9:
      if ((in_esi[0x5a] & 0x3000000U) != 0) {
        bVar4 = (in_esi[0x5a] & 0x10U) == 0;
        goto LAB_0041e8b2;
      }
      break;
    case 10:
      if ((in_esi[0x5a] & 0x3000000U) != 0) {
        bVar4 = (in_esi[0x5a] & 0x20U) == 0;
        goto LAB_0041e8b2;
      }
      break;
    case 0xb:
      if ((in_esi[0x5a] & 0x3000000U) != 0) {
        bVar4 = (in_esi[0x5a] & 0x40U) == 0;
        goto LAB_0041e8b2;
      }
      break;
    case 0xc:
      if (((in_esi[0x5a] & 0x3000000U) == 0) || (-1 < (char)in_esi[0x5a])) break;
      goto switchD_0041e888_caseD_0;
    case 0xd:
      if ((in_esi[0x5a] & 0x3000000U) != 0) {
        bVar4 = (in_esi[0x5a] & 0x100U) == 0;
        goto LAB_0041e8b2;
      }
      break;
    case 0xe:
      if ((in_esi[0x5a] & 0x3000000U) != 0) {
        bVar4 = (in_esi[0x5a] & 0x200U) == 0;
        goto LAB_0041e8b2;
      }
      break;
    case 0xf:
      if ((in_esi[0x5a] & 0x3000000U) != 0) {
        bVar4 = (in_esi[0x5a] & 0x400U) == 0;
        goto LAB_0041e8b2;
      }
      break;
    case 0x10:
      if ((in_esi[0x5a] & 0x3000000U) != 0) {
        bVar4 = (in_esi[0x5a] & 0x800U) == 0;
        goto LAB_0041e8b2;
      }
      break;
    case 0x16:
      if ((in_esi[0x5a] & 0x2000000U) != 0) {
        bVar4 = (in_esi[0x5a] & 0x1000U) == 0;
        goto LAB_0041e8b2;
      }
      break;
    case 0x17:
      if ((in_esi[0x5a] & 0x2000000U) != 0) {
        bVar4 = (in_esi[0x5a] & 0x2000U) == 0;
        goto LAB_0041e8b2;
      }
      break;
    case 0x18:
      if ((in_esi[0x5a] & 0x2000000U) != 0) {
        bVar4 = (in_esi[0x5a] & 0x4000U) == 0;
        goto LAB_0041e8b2;
      }
      break;
    case 0x19:
      if (((in_esi[0x5a] & 0x2000000U) == 0) || (-1 < (char)((uint)in_esi[0x5a] >> 8))) break;
      uVar2 = 1;
      goto LAB_0041ea14;
    case 0x1a:
      if ((in_esi[0x5a] & 0x2000000U) != 0) {
        bVar4 = (in_esi[0x5a] & 0x10000U) == 0;
        goto LAB_0041e8b2;
      }
      break;
    case 0x1b:
      if ((in_esi[0x5a] & 0x2000000U) != 0) {
        bVar4 = (in_esi[0x5a] & 0x20000U) == 0;
        goto LAB_0041e8b2;
      }
      break;
    case 0x1c:
      uVar2 = in_esi[0x5a];
      uVar1 = uVar2 & 0x800000;
LAB_0041e8aa:
      if (uVar1 != 0) {
        bVar4 = (uVar2 & 1) == 0;
LAB_0041e8b2:
        if (!bVar4) goto switchD_0041e888_caseD_0;
      }
      break;
    case 0x1d:
      if (((in_esi[0x5a] & 0x800000U) == 0) || ((in_esi[0x5a] & 2U) == 0)) break;
      uVar2 = 1;
      goto LAB_0041ea14;
    case 0x1e:
      if (((in_esi[0x5a] & 0x800000U) == 0) || ((in_esi[0x5a] & 4U) == 0)) break;
      uVar2 = 1;
      goto LAB_0041ea14;
    case 0x1f:
      if (((in_esi[0x5a] & 0x800000U) == 0) || ((in_esi[0x5a] & 8U) == 0)) break;
      uVar2 = 1;
      goto LAB_0041ea14;
    case 0x20:
      if (((in_esi[0x5a] & 0x800000U) == 0) || ((in_esi[0x5a] & 0x10U) == 0)) break;
      uVar2 = 1;
      goto LAB_0041ea14;
    case 0x21:
      if (((in_esi[0x5a] & 0x800000U) == 0) || ((in_esi[0x5a] & 0x20U) == 0)) break;
      uVar2 = 1;
      goto LAB_0041ea14;
    }
    uVar2 = 0;
LAB_0041ea14:
    in_esi[points + 4] = uVar2;
LAB_0041ea18:
    points = points + 1;
    if (0x27 < points) {
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
      RwMatrixTranslate(&frame.local_40,(RwV3d *)(in_esi + 0x54),rwCOMBINEREPLACE);
      RwFrameTransform((RwFrame *)in_esi[0x58],&frame.local_40,rwCOMBINEREPLACE);
      RwFrameScale((RwFrame *)in_esi[0x58],&frame.local_4c,rwCOMBINEPRECONCAT);
      points = player_get_points(*in_esi);
      iVar3 = score_target_a();
      frame.local_4c.x = (float)points / (float)iVar3;
      frame.local_4c.z = 1.0;
      frame.local_4c.y = 1.0;
      RwFrameScale(*(RwFrame **)(in_esi[0x2e] + 4),&frame.local_4c,rwCOMBINEREPLACE);
      return;
    }
  } while( true );
switchD_0041e888_caseD_0:
  uVar2 = 1;
  goto LAB_0041ea14;
}

/* entry thunk at the original address of team_panel_entry_update: register inputs (esi) -> natural C team_panel_entry_update_n; ret 0 */
__declspec(naked) void team_panel_entry_update(void)
{
    __asm {
        push esi
        call team_panel_entry_update_n
        add esp, 4
        ret
    }
}
