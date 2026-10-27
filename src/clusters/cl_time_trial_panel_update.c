// cluster-entries: time_trial_panel_update
/* Installed cluster time_trial_panel_update (tools/cluster_install.py from output/cluster_pilot_zx/time_trial_panel_update.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/* DECLS-BEGIN */
/* DECLS-END */
/*0041d520*/ extern int btb_panel_slot_set_matrix();
/*0041d650*/ extern int btb_panel_slot_update();
/*0041da50*/ extern undefined __cdecl time_trial_panel_step_part_a(undefined4 p0, undefined4 p1);
/*0041db40*/ extern int time_trial_panel_wheel_spin();
/*00658d18*/ extern undefined4 g_hudpanel_658d18[0x1000];
extern char g_score_658d98[];
#define PANEL_SLOT_DWORDS (0x10) /* dwords copied per HUD slot, one 4x4 matrix (inferred) */
#define SLOT_PTR_BYTES (4) /* size of one slot matrix pointer in slot_matrix_table */

/* time_trial_panel_update
 * Runs the time_trial_panel_wheel_spin, time_trial_panel_step_part_b and time_trial_panel_step_part_a
 * helpers, then for each HUD slot (two: g_score_658d98 is 0x80 bytes past g_hudpanel_658d18) sets the
 * slot matrix from slot_matrix_table, runs btb_panel_slot_update and copies the slot's matrix into
 * g_hudpanel_658d18.
 * slot_matrix_table: array of pointers to the per-slot matrices (inferred).
 * unused_arg: never read.
 * Returns nothing.
 * (was: "runs the three time_trial_record_step helpers and sets the BTB panel slot matrices";
 *  the callees are time_trial_panel_* helpers, not time_trial_record_step) */

static int time_trial_panel_step_part_b_r(int p0);
extern void time_trial_panel_step_part_b(void);
void __cdecl time_trial_panel_update(int slot_matrix_table,char *unused_arg)

{
  int copy_left;
  int slot_index; /* first holds slot_matrix_table for the step_part_a call, then the slot index */
  undefined4 *hud_dest;
  undefined4 *next_hud_slot;
  undefined4 *src_matrix;

  slot_index = slot_matrix_table;
  time_trial_panel_wheel_spin(slot_matrix_table);
  time_trial_panel_step_part_b_r(slot_matrix_table);
  time_trial_panel_step_part_a(slot_matrix_table,slot_index);
  slot_index = 0;
  hud_dest = g_hudpanel_658d18;
  /* loop over slots: set and update the slot, then copy its matrix into the HUD buffer */
  do {
    src_matrix = *(undefined4 **)(slot_matrix_table + slot_index * SLOT_PTR_BYTES);
    btb_panel_slot_set_matrix(slot_index,src_matrix);
    btb_panel_slot_update(slot_index);
    next_hud_slot = hud_dest + PANEL_SLOT_DWORDS;
    slot_index = slot_index + 1;
    /* copy one slot matrix (PANEL_SLOT_DWORDS dwords) into the HUD buffer */
    for (copy_left = PANEL_SLOT_DWORDS; copy_left != 0; copy_left = copy_left + -1) {
      *hud_dest = *src_matrix;
      src_matrix = src_matrix + 1;
      hud_dest = hud_dest + 1;
    }
    hud_dest = next_hud_slot;
  } while ((int)next_hud_slot < ((int)&g_score_658d98));
  return;
}

/* time_trial_panel_step_part_b_r: C -> original time_trial_panel_step_part_b (registers edi) */
__declspec(naked) int time_trial_panel_step_part_b_r(int p0)
{
    __asm {
        push edi
        mov edi, dword ptr [esp + 8]
        call time_trial_panel_step_part_b
        pop edi
        ret
    }
}
