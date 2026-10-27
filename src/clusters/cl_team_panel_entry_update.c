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

/* panel[0x5a] is the flag word (bit 0x40000 toggled, 0x3000000/0x800000 select button groups); the loop below tests each button bit and updates the entry. */
void team_panel_entry_update_n(int *panel)

{
  uint uVar1;
  uint uVar2;
  int score_target;
  int points;
  bool bit_clear;
  struct {
    RwV3d unit_scale;
    RwMatrix mtx;
  } frame;
  
  frame.unit_scale.x = 1.0;
  frame.unit_scale.y = 1.0;
  frame.unit_scale.z = 1.0;
  if (*(int *)((char *)&g_team_6592b0 + *panel * 4) == 0) {
    uVar2 = panel[0x5a] & 0xfffbffff;
  }
  else {
    uVar2 = panel[0x5a] | 0x40000;
  }
  panel[0x5a] = uVar2;
  points = 0;
  do {
    if (panel[points + 0x2c] == 0) goto LAB_0041ea18;
    switch(points) {
    case 0:
    case 1:
    case 2:
    case 3:
      goto switchD_0041e888_caseD_0;
    case 4:
      uVar2 = panel[0x5a] & 0x40000;
      goto LAB_0041ea14;
    case 5:
      uVar2 = panel[0x5a];
      uVar1 = uVar2 & 0x3000000;
      goto LAB_0041e8aa;
    case 6:
      if ((panel[0x5a] & 0x3000000U) == 0) break;
      bit_clear = (panel[0x5a] & 2U) == 0;
      goto LAB_0041e8b2;
    case 7:
      if ((panel[0x5a] & 0x3000000U) != 0) {
        bit_clear = (panel[0x5a] & 4U) == 0;
        goto LAB_0041e8b2;
      }
      break;
    case 8:
      if ((panel[0x5a] & 0x3000000U) != 0) {
        bit_clear = (panel[0x5a] & 8U) == 0;
        goto LAB_0041e8b2;
      }
      break;
    case 9:
      if ((panel[0x5a] & 0x3000000U) != 0) {
        bit_clear = (panel[0x5a] & 0x10U) == 0;
        goto LAB_0041e8b2;
      }
      break;
    case 10:
      if ((panel[0x5a] & 0x3000000U) != 0) {
        bit_clear = (panel[0x5a] & 0x20U) == 0;
        goto LAB_0041e8b2;
      }
      break;
    case 0xb:
      if ((panel[0x5a] & 0x3000000U) != 0) {
        bit_clear = (panel[0x5a] & 0x40U) == 0;
        goto LAB_0041e8b2;
      }
      break;
    case 0xc:
      if (((panel[0x5a] & 0x3000000U) == 0) || (-1 < (char)panel[0x5a])) break;
      goto switchD_0041e888_caseD_0;
    case 0xd:
      if ((panel[0x5a] & 0x3000000U) != 0) {
        bit_clear = (panel[0x5a] & 0x100U) == 0;
        goto LAB_0041e8b2;
      }
      break;
    case 0xe:
      if ((panel[0x5a] & 0x3000000U) != 0) {
        bit_clear = (panel[0x5a] & 0x200U) == 0;
        goto LAB_0041e8b2;
      }
      break;
    case 0xf:
      if ((panel[0x5a] & 0x3000000U) != 0) {
        bit_clear = (panel[0x5a] & 0x400U) == 0;
        goto LAB_0041e8b2;
      }
      break;
    case 0x10:
      if ((panel[0x5a] & 0x3000000U) != 0) {
        bit_clear = (panel[0x5a] & 0x800U) == 0;
        goto LAB_0041e8b2;
      }
      break;
    case 0x16:
      if ((panel[0x5a] & 0x2000000U) != 0) {
        bit_clear = (panel[0x5a] & 0x1000U) == 0;
        goto LAB_0041e8b2;
      }
      break;
    case 0x17:
      if ((panel[0x5a] & 0x2000000U) != 0) {
        bit_clear = (panel[0x5a] & 0x2000U) == 0;
        goto LAB_0041e8b2;
      }
      break;
    case 0x18:
      if ((panel[0x5a] & 0x2000000U) != 0) {
        bit_clear = (panel[0x5a] & 0x4000U) == 0;
        goto LAB_0041e8b2;
      }
      break;
    case 0x19:
      if (((panel[0x5a] & 0x2000000U) == 0) || (-1 < (char)((uint)panel[0x5a] >> 8))) break;
      uVar2 = 1;
      goto LAB_0041ea14;
    case 0x1a:
      if ((panel[0x5a] & 0x2000000U) != 0) {
        bit_clear = (panel[0x5a] & 0x10000U) == 0;
        goto LAB_0041e8b2;
      }
      break;
    case 0x1b:
      if ((panel[0x5a] & 0x2000000U) != 0) {
        bit_clear = (panel[0x5a] & 0x20000U) == 0;
        goto LAB_0041e8b2;
      }
      break;
    case 0x1c:
      uVar2 = panel[0x5a];
      uVar1 = uVar2 & 0x800000;
LAB_0041e8aa:
      if (uVar1 != 0) {
        bit_clear = (uVar2 & 1) == 0;
LAB_0041e8b2:
        if (!bit_clear) goto switchD_0041e888_caseD_0;
      }
      break;
    case 0x1d:
      if (((panel[0x5a] & 0x800000U) == 0) || ((panel[0x5a] & 2U) == 0)) break;
      uVar2 = 1;
      goto LAB_0041ea14;
    case 0x1e:
      if (((panel[0x5a] & 0x800000U) == 0) || ((panel[0x5a] & 4U) == 0)) break;
      uVar2 = 1;
      goto LAB_0041ea14;
    case 0x1f:
      if (((panel[0x5a] & 0x800000U) == 0) || ((panel[0x5a] & 8U) == 0)) break;
      uVar2 = 1;
      goto LAB_0041ea14;
    case 0x20:
      if (((panel[0x5a] & 0x800000U) == 0) || ((panel[0x5a] & 0x10U) == 0)) break;
      uVar2 = 1;
      goto LAB_0041ea14;
    case 0x21:
      if (((panel[0x5a] & 0x800000U) == 0) || ((panel[0x5a] & 0x20U) == 0)) break;
      uVar2 = 1;
      goto LAB_0041ea14;
    }
    uVar2 = 0;
LAB_0041ea14:
    panel[points + 4] = uVar2;
LAB_0041ea18:
    points = points + 1;
    if (0x27 < points) {
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
      RwMatrixTranslate(&frame.mtx,(RwV3d *)(panel + 0x54),rwCOMBINEREPLACE);
      RwFrameTransform((RwFrame *)panel[0x58],&frame.mtx,rwCOMBINEREPLACE);
      RwFrameScale((RwFrame *)panel[0x58],&frame.unit_scale,rwCOMBINEPRECONCAT);
      points = player_get_points(*panel);
      score_target = score_target_a();
      frame.unit_scale.x = (float)points / (float)score_target;
      frame.unit_scale.z = 1.0;
      frame.unit_scale.y = 1.0;
      RwFrameScale(*(RwFrame **)(panel[0x2e] + 4),&frame.unit_scale,rwCOMBINEREPLACE);
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
