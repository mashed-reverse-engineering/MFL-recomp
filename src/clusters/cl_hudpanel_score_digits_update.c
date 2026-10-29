// cluster-entries: hudpanel_score_digits_update
/* Installed cluster hudpanel_score_digits_update (tools/cluster_install.py from output/cluster_pilot_zx/hudpanel_score_digits_update.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/* DECLS-BEGIN */
/* DECLS-END */
/*00434bb0*/ extern int get_event_type();
/*004d1500*/ extern struct RwFrame * RwFrameScale(struct RwFrame *p0, struct RwV3d *p1, enum RwOpCombineType p2);
/*004d15a0*/ extern struct RwFrame * RwFrameTransform(struct RwFrame *p0, RwMatrix *p1, enum RwOpCombineType p2);
/*004d4870*/ extern RwMatrix * RwMatrixTranslate(RwMatrix *p0, struct RwV3d *p1, enum RwOpCombineType p2);
/*005dd32c*/ extern float k_f_3;
/*005dd330*/ extern float float_1_0;
/*005dd634*/ extern float k_f_2;
/*005ddc1c*/ extern float k_f_0_33333334;
/*005e64a4*/ extern float k_f_0;
/*00659ac0*/ extern undefined4 g_hudpanel_659ac0;
/*00659ac8*/ extern undefined4 g_race_update_659ac8;
/*00659b04*/ extern undefined4 g_ai_659b04;
/*00659b0c*/ extern float g_race_update_659b0c;
/*007ef804*/ extern undefined4 g_tick_number;

#define MATRIX_FLAGS_IDENTITY (0x20003) /* inferred: RwMatrix flags, the matrix is rebuilt as identity below */
#define TICK_BLINK_MASK (0x10) /* bit of g_tick_number that makes the bar flicker */



/* hudpanel_score_digits_update */

/* "positions the panel (0.48,0.35) and scales the bar to 0, 1/3, 2/3, 1 by the counter" */

/* Per-frame update of the score digits HUD panel (inferred from the body and the callee names).
 * Refreshes the digit counter, places the panel at (0.48, 0.35) on the stopclock panel frame (g_ai_659b04 holds clump->frame), then sets the
 * bar's X scale from the g_race_update_659b0c ratio and applies it to the frame at +4 of
 * g_race_update_659ac8. No parameters, no return value. */
static int ai_counters_snapshot_r(int p0);
extern void ai_counters_snapshot(void);
static int stopclock_set_digit_count_r(int p0);
extern void stopclock_set_digit_count(void);
void hudpanel_score_digits_update(void)

{
  int event_type;
  RwV3d bar_scale;
  RwV3d panel_offset;
  RwMatrix panel_matrix;

  panel_offset.x = 0.48;
  panel_offset.y = 0.35;
  panel_offset.z = 1.0;
  bar_scale.x = 1.0;
  bar_scale.y = 1.0;
  bar_scale.z = 1.0;
  event_type = get_event_type();
  /* The training event (EVENT_TRAINING) sets the digit count with stopclock_set_digit_count; every other event uses ai_counters_snapshot. */
  if (event_type == EVENT_TRAINING) {
    stopclock_set_digit_count_r(&g_hudpanel_659ac0);
  }
  else {
    ai_counters_snapshot_r(&g_hudpanel_659ac0);
  }
  /* Build an identity matrix, move it to panel_offset, and attach it to the stopclock panel frame (g_ai_659b04). */
  panel_matrix.flags = panel_matrix.flags | MATRIX_FLAGS_IDENTITY;
  panel_matrix.at.z = 1.0;
  panel_matrix.up.y = 1.0;
  panel_matrix.right.x = 1.0;
  panel_matrix.up.x = 0.0;
  panel_matrix.right.z = 0.0;
  panel_matrix.right.y = 0.0;
  panel_matrix.at.y = 0.0;
  panel_matrix.at.x = 0.0;
  panel_matrix.up.z = 0.0;
  panel_matrix.pos.z = 0.0;
  panel_matrix.pos.y = 0.0;
  panel_matrix.pos.x = 0.0;
  RwMatrixTranslate(&panel_matrix,&panel_offset,rwCOMBINEREPLACE);
  RwFrameTransform(g_ai_659b04,&panel_matrix,rwCOMBINEREPLACE);
  RwFrameScale(g_ai_659b04,&bar_scale,rwCOMBINEPRECONCAT);
  /* Bar step from g_race_update_659b0c: 0, 1/3, 2/3 or 1 against float_1_0, k_f_2, k_f_3. */
  if (g_race_update_659b0c < float_1_0) {
    bar_scale.x = 0.0;
  }
  else if (g_race_update_659b0c < k_f_2) {
    bar_scale.x = 0.33333334;
  }
  else {
    bar_scale.x = 0.6666667;
    if (!(g_race_update_659b0c < k_f_3)) {
      bar_scale.x = 1.0;
    }
  }
  /* Blink: add 1/3 to the bar on the ticks where TICK_BLINK_MASK is set. */
  if (((byte)g_tick_number & TICK_BLINK_MASK) != 0) {
    bar_scale.x = bar_scale.x + k_f_0_33333334;
  }
  /* Clamp the bar value to 0..1 (below 0 gives 0, 1 or more gives 1). */
  if (float_1_0 <= bar_scale.x) {
    bar_scale.x = 1.0;
  }
  else {
    if (!(k_f_0 <= bar_scale.x)) {
      bar_scale.x = 0.0;
    }
  }
  bar_scale.z = 1.0;
  bar_scale.y = 1.0;
  RwFrameScale(*(RwFrame **)(g_race_update_659ac8 + 4),&bar_scale,rwCOMBINEREPLACE);
  return;
}

/* ai_counters_snapshot_r: C -> original ai_counters_snapshot (registers edi) */
__declspec(naked) int ai_counters_snapshot_r(int p0)
{
    __asm {
        push edi
        mov edi, dword ptr [esp + 8]
        call ai_counters_snapshot
        pop edi
        ret
    }
}

/* stopclock_set_digit_count_r: C -> original stopclock_set_digit_count (registers esi) */
__declspec(naked) int stopclock_set_digit_count_r(int p0)
{
    __asm {
        push esi
        mov esi, dword ptr [esp + 8]
        call stopclock_set_digit_count
        pop esi
        ret
    }
}
