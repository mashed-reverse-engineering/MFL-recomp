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

/* Per frame update of one endpoint panel entry (esi = entry, stride 0x160; entry[0] = player index, entry[3] = count of listed players, entry[4..0x2b] = 40 indicator slots, entry[0x55] = RwFrame, entry[0x57] = state flag word, bit 0x40000 = highlighted). Sets the highlight flag, stores the player's points change class (0,1,2 up; -1,-2 down) into flag bits 0x4000000..0x40000000, then derives each indicator slot (0 or 1) from those flags, positions the panel frame and sets the score bar scale (points / score_target_a) and the pulsing scale of the highlight (inferred). */
void endpoint_panel_entry_update_n(int *entry)

{
  uint masked_flags;
  undefined4 points_change;
  uint flags;
  int idx;
  int tmp;
  bool bit_clear;
  float10 pulse;
  struct {
    RwV3d scale;
    RwMatrix xform;
  } frame;
  
  frame.scale.x = 1.0;
  frame.scale.y = 1.0;
  frame.scale.z = 1.0;
  if (*(int *)((char *)&g_endpoint_panel_659798 + *entry * 4) == 0) {
    flags = entry[0x57] & 0xfffbffff;
  }
  else {
    flags = entry[0x57] | 0x40000;
  }
  entry[0x57] = flags;
  points_change = player_get_points_change_visual(*entry);
  flags = entry[0x57] & 0x83ffffff;
  entry[0x57] = flags;
  /* stored change class selects exactly one of the flag bits 0x4000000 (2), 0x8000000 (1), 0x10000000 (0), 0x20000000 (-1), 0x40000000 (-2) */
  switch(points_change) {
  case 0:
    flags = flags | 0x10000000;
    break;
  case 1:
    flags = flags | 0x8000000;
    break;
  case 2:
    flags = flags | 0x4000000;
    break;
  case 0xfffffffe:
    flags = flags | 0x40000000;
    break;
  case 0xffffffff:
    flags = flags | 0x20000000;
    break;
  default:
    goto switchD_0041fee5_default;
  }
  entry[0x57] = flags;
switchD_0041fee5_default:
  idx = 0;
  do {
    /* for each of the 40 indicator slots that is in use, compute its 0/1 value from the flag word; the switch is the per slot rule */
    if (entry[idx + 0x2c] == 0) goto LAB_004200c8;
    switch(idx) {
    case 0:
    case 1:
    case 2:
    case 3:
      goto switchD_0041ff38_caseD_0;
    case 4:
      flags = entry[0x57] & 0x40000;
      goto LAB_004200c4;
    case 5:
      flags = entry[0x57];
      masked_flags = flags & 0x3000000;
      goto LAB_0041ff5a;
    case 6:
      if ((entry[0x57] & 0x3000000U) == 0) break;
      bit_clear = (entry[0x57] & 2U) == 0;
      goto LAB_0041ff62;
    case 7:
      if ((entry[0x57] & 0x3000000U) != 0) {
        bit_clear = (entry[0x57] & 4U) == 0;
        goto LAB_0041ff62;
      }
      break;
    case 8:
      if ((entry[0x57] & 0x3000000U) != 0) {
        bit_clear = (entry[0x57] & 8U) == 0;
        goto LAB_0041ff62;
      }
      break;
    case 9:
      if ((entry[0x57] & 0x3000000U) != 0) {
        bit_clear = (entry[0x57] & 0x10U) == 0;
        goto LAB_0041ff62;
      }
      break;
    case 10:
      if ((entry[0x57] & 0x3000000U) != 0) {
        bit_clear = (entry[0x57] & 0x20U) == 0;
        goto LAB_0041ff62;
      }
      break;
    case 0xb:
      if ((entry[0x57] & 0x3000000U) != 0) {
        bit_clear = (entry[0x57] & 0x40U) == 0;
        goto LAB_0041ff62;
      }
      break;
    case 0xc:
      if (((entry[0x57] & 0x3000000U) == 0) || (-1 < (char)entry[0x57])) break;
      goto switchD_0041ff38_caseD_0;
    case 0xd:
      if ((entry[0x57] & 0x3000000U) != 0) {
        bit_clear = (entry[0x57] & 0x100U) == 0;
        goto LAB_0041ff62;
      }
      break;
    case 0xe:
      if ((entry[0x57] & 0x3000000U) != 0) {
        bit_clear = (entry[0x57] & 0x200U) == 0;
        goto LAB_0041ff62;
      }
      break;
    case 0xf:
      if ((entry[0x57] & 0x3000000U) != 0) {
        bit_clear = (entry[0x57] & 0x400U) == 0;
        goto LAB_0041ff62;
      }
      break;
    case 0x10:
      if ((entry[0x57] & 0x3000000U) != 0) {
        bit_clear = (entry[0x57] & 0x800U) == 0;
        goto LAB_0041ff62;
      }
      break;
    case 0x11:
      flags = entry[0x57] & 0x4000000;
      goto LAB_004200c4;
    case 0x12:
      flags = entry[0x57] & 0x8000000;
      goto LAB_004200c4;
    case 0x13:
      flags = entry[0x57] & 0x10000000;
      goto LAB_004200c4;
    case 0x14:
      flags = entry[0x57] & 0x20000000;
      goto LAB_004200c4;
    case 0x15:
      flags = entry[0x57] & 0x40000000;
      goto LAB_004200c4;
    case 0x16:
      if ((entry[0x57] & 0x2000000U) != 0) {
        bit_clear = (entry[0x57] & 0x1000U) == 0;
        goto LAB_0041ff62;
      }
      break;
    case 0x17:
      if ((entry[0x57] & 0x2000000U) != 0) {
        bit_clear = (entry[0x57] & 0x2000U) == 0;
        goto LAB_0041ff62;
      }
      break;
    case 0x18:
      if ((entry[0x57] & 0x2000000U) != 0) {
        bit_clear = (entry[0x57] & 0x4000U) == 0;
        goto LAB_0041ff62;
      }
      break;
    case 0x19:
      if (((entry[0x57] & 0x2000000U) == 0) || (-1 < (char)((uint)entry[0x57] >> 8))) break;
      flags = 1;
      goto LAB_004200c4;
    case 0x1a:
      if ((entry[0x57] & 0x2000000U) != 0) {
        bit_clear = (entry[0x57] & 0x10000U) == 0;
        goto LAB_0041ff62;
      }
      break;
    case 0x1b:
      if ((entry[0x57] & 0x2000000U) != 0) {
        bit_clear = (entry[0x57] & 0x20000U) == 0;
        goto LAB_0041ff62;
      }
      break;
    case 0x1c:
      flags = entry[0x57];
      masked_flags = flags & 0x800000;
LAB_0041ff5a:
      if (masked_flags != 0) {
        bit_clear = (flags & 1) == 0;
LAB_0041ff62:
        if (!bit_clear) goto switchD_0041ff38_caseD_0;
      }
      break;
    case 0x1d:
      if (((entry[0x57] & 0x800000U) == 0) || ((entry[0x57] & 2U) == 0)) break;
      flags = 1;
      goto LAB_004200c4;
    case 0x1e:
      if (((entry[0x57] & 0x800000U) == 0) || ((entry[0x57] & 4U) == 0)) break;
      flags = 1;
      goto LAB_004200c4;
    case 0x1f:
      if (((entry[0x57] & 0x800000U) == 0) || ((entry[0x57] & 8U) == 0)) break;
      flags = 1;
      goto LAB_004200c4;
    case 0x20:
      if (((entry[0x57] & 0x800000U) == 0) || ((entry[0x57] & 0x10U) == 0)) break;
      flags = 1;
      goto LAB_004200c4;
    case 0x21:
      if (((entry[0x57] & 0x800000U) == 0) || ((entry[0x57] & 0x20U) == 0)) break;
      flags = 1;
      goto LAB_004200c4;
    }
    flags = 0;
LAB_004200c4:
    entry[idx + 4] = flags;
LAB_004200c8:
    idx = idx + 1;
  } while (idx < 0x28);
  idx = 0;
  /* find the player's own position (g_endpoint_panel_6597a8) in the entry's player list; tmp = 0 when found, else 1 selects the translation row */
  tmp = 1;
  if (0 < entry[3]) {
    do {
      if (entry[idx] == g_endpoint_panel_6597a8) {
        tmp = 0;
        break;
      }
      idx = idx + 1;
    } while (idx < entry[3]);
  }
  frame.xform.flags = frame.xform.flags | 0x20003;
  frame.xform.at.z = 1.0;
  frame.xform.up.y = 1.0;
  frame.xform.right.x = 1.0;
  frame.xform.up.x = 0.0;
  frame.xform.right.z = 0.0;
  frame.xform.right.y = 0.0;
  frame.xform.at.y = 0.0;
  frame.xform.at.x = 0.0;
  frame.xform.up.z = 0.0;
  frame.xform.pos.z = 0.0;
  frame.xform.pos.y = 0.0;
  frame.xform.pos.x = 0.0;
  RwMatrixTranslate(&frame.xform,(RwV3d *)((char *)&g_endpoint_panel_603f94 + tmp * 0xc),rwCOMBINEREPLACE);
  RwFrameTransform((RwFrame *)entry[0x55],&frame.xform,rwCOMBINEREPLACE);
  frame.scale.x = frame.scale.x * k_f_1_125;
  frame.scale.y = frame.scale.y * k_f_1_125;
  frame.scale.z = frame.scale.z * k_f_1_125;
  RwFrameScale((RwFrame *)entry[0x55],&frame.scale,rwCOMBINEPRECONCAT);
  idx = player_get_points(*entry);
  tmp = score_target_a();
  frame.scale.x = (float)idx / (float)tmp;
  frame.scale.z = 1.0;
  frame.scale.y = 1.0;
  RwFrameScale(*(RwFrame **)(entry[0x2e] + 4),&frame.scale,rwCOMBINEREPLACE);
  if ((entry[0x57] & 0x40000U) == 0) {
    frame.scale.y = 1.0;
    frame.scale.x = 1.0;
  }
  else {
    pulse = (float10)(int)g_endpoint_659a78;
    if ((int)g_endpoint_659a78 < 0) {
      pulse = pulse + (float10)k_f_4_2949673e9;
    }
    pulse = (float10)sin(pulse * (float10)k_f_0_2);
    pulse = pulse * (float10)k_f_0_15 + (float10)float_1_0;
    frame.scale.y = (RwReal)pulse;
    frame.scale.x = (RwReal)pulse;
  }
  frame.scale.z = 1.0;
  RwFrameScale(*(RwFrame **)(entry[0x30] + 4),&frame.scale,rwCOMBINEREPLACE);
  return;
switchD_0041ff38_caseD_0:
  flags = 1;
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
