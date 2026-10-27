#include "mfl_tier2.h"
/*00434a10*/ extern int is_team_mode();
/*007f01fc*/ extern struct PlayerSlot g_player_slots[4];
/*008c8fc0*/ extern struct PlayerStatRecord g_player_stats_result[4];
/* car_damage_at_local_point */
/* "(car, amount, local position): damage to the car part nearest the point (inferred;
   damage_add_clamped is the clamp at 50.0)" */
/* car_damage_at_world_point */
/* "(car, amount, world position): transforms the point into car space with the inverse matrix and
   calls car_damage_at_local_point" */
/* debris_slot_clear_forward */
/* "calls debris_slot_clear" */
/* debris_slot_list_update_visible */
/* "for the 10 slots (stride 0x18) with a clump calls game_fn_424ab0" */
/* debris_any_active_all_vehicles */
/* "loops vehicle debris records calling vehicles_any_active_sub_424db0" */
/* vehicle_tyre_marks_render */
/* WARNING: Function: __chkstk replaced with injection: alloca_probe */
/* "builds an Im3D vertex list from the tyre mark segments (0xF54 byte records) and renders them" */
/* player_stat_add_turbo_used */
/* "(player, amount): g_player_stats_race[player].turbo_used += amount (called when the turbo charge
   is spent)" */
/* car_kill */
/* "(car): car_init, inactive, damage set to 50.0 on both sides, weapon status 2, no HUD in game
   mode 7" */
/* ai_editor_update */
/* "runs the three AI editor input handlers when player slot 3 is the editor slot" */
/* player_stat_get_turbo_used_team */
/* "kind 2 'Turbo Skills': result table +0xFC, summed over the team" */
/*
 * Returns the 'Turbo Skills' value (turbo_used of the result table) for one player.
 * player_index: 0-3, index into g_player_stats_result / g_player_slots.
 * Solo: that player's own turbo_used. Team mode: summed over every player whose slot_state
 * matches this player's (inferred: same team), this player counted once.
 */
int player_stat_get_turbo_used_team(int player_index)
{
  int slot_state;
  int turbo_used_team;

  /* is_team_mode() result first (0 = solo), then overwritten with the turbo_used value */
  turbo_used_team = is_team_mode();
  if (turbo_used_team != 0) {
    slot_state = g_player_slots[player_index].state;
    turbo_used_team = g_player_stats_result[player_index].turbo_used;
    if ((g_player_slots[0].state == slot_state) && (player_index != 0)) {
      turbo_used_team = turbo_used_team + g_player_stats_result[0].turbo_used;
    }
    if ((g_player_slots[1].state == slot_state) && (player_index != 1)) {
      turbo_used_team = turbo_used_team + g_player_stats_result[1].turbo_used;
    }
    if ((g_player_slots[2].state == slot_state) && (player_index != 2)) {
      turbo_used_team = turbo_used_team + g_player_stats_result[2].turbo_used;
    }
    if ((g_player_slots[3].state == slot_state) && (player_index != 3)) {
      turbo_used_team = turbo_used_team + g_player_stats_result[3].turbo_used;
    }
  }
  else {
    turbo_used_team = g_player_stats_result[player_index].turbo_used;
  }
  return turbo_used_team;
}
