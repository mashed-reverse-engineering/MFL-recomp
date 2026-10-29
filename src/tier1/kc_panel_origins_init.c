#include "mfl_tier2.h"
/*00603f4c*/ extern undefined4 g_kc_panel_origin_table;
/*00659790*/ extern undefined4 g_kc_659790;
#define KC_PANEL_ORIGIN_FLOATS (12) /* four panel origins of three floats each (x, y, z) */
/* btb_panel_destroy */
/* "RpClumpDestroy(0x658B40) (BTBPanel.dff, inferred)" */
/* hudpanel_render_if_loaded_b */
/* "hudpanel_render_group when g_hudpanel_658b48 is set" */
/* btb_panel_slot_set_matrix */
/* "(slot, matrix*): copies 16 floats" */
/* btb_panel_slot_render */
/* "calls the clump object's method +0x48 when visible" */
/* load_ghost_model */
/* ghost.dff */
/* time_trial_panel_step_part_a */
/* "per slot direction vector between car and panel point, normalised length" */
/* time_trial_panel_wheel_spin */
/* "per slot spin angle of four parts from distance*direction, wrapped at 360" */
/* score_panel_clear */
/* "g_score_658fd0 = 0" */
/* hudpanel_fuel_meter_update */
/* "places the panel frame (-0.48,-0.35) scale 1.25 and scales the bar by 1 - fx value" */
/* hudpanel_clump_destroy */
/* "RpClumpDestroy(g_hudpanel_6592d4)" */
/* hudpanel_render_if_loaded */
/* "hudpanel_render_group when g_hudpanel_6592dc is set" */
/* kc_panel_origins_init */
/* (was named team_colour_table_init_4: "fills 12 floats (four team colours)"; the values are positions: cl_kc_panel_entry_update uses entry slot_index * 12 as a RwMatrixTranslate vector) */
/*
 * Fills the origin table of the four KC panels at g_kc_panel_origin_table with KC_PANEL_ORIGIN_FLOATS floats
 * (four origins of three floats each), then clears g_kc_659790.
 * Takes no parameters and returns nothing. The floats decode as
 * (0.48, 0.25, 1.0), (0.48, 0.15, 1.0), (0.48, 0.05, 1.0), (0.48, -0.05, 1.0)
 * (x 0.48, y stepping down by 0.1, z 1.0: panel positions, not colours).
 */
void kc_panel_origins_init(void)
{
  int remaining;
  undefined4 *src;
  undefined4 *dst;
  float origin_table [KC_PANEL_ORIGIN_FLOATS];

  /* Build the float values in the local table. */
  origin_table[0] = 0.48f;
  origin_table[1] = 0.25f;
  origin_table[2] = 1.0f;
  origin_table[3] = 0.48f;
  origin_table[4] = 0.15f;
  origin_table[5] = 1.0f;
  origin_table[6] = 0.48f;
  origin_table[7] = 0.05f;
  origin_table[8] = 1.0f;
  origin_table[9] = 0.48f;
  origin_table[10] = -0.05f;
  origin_table[0xb] = 1.0f;
  src = (undefined4 *)origin_table;
  dst = &g_kc_panel_origin_table;
  /* Copy KC_PANEL_ORIGIN_FLOATS floats from the local table into the global table. */
  for (remaining = KC_PANEL_ORIGIN_FLOATS; remaining != 0; remaining = remaining + -1) {
    *dst = *src;
    src = src + 1;
    dst = dst + 1;
  }
  g_kc_659790 = 0;
  return;
}
