#include "mfl_tier2.h"
/*0040fdb0*/ extern int get_player_count();
/*0040ff20*/ extern int race_setup_get_slot_type();
/*0041b810*/ extern int race_mark_ai_slots();
/*0041b9d0*/ extern int hudpanel_slot_init_player();
/*004222e0*/ extern int vehicle_get_matrix_ltm_b();
/*00423fe0*/ extern undefined damage_add_clamped(undefined4 p0, undefined4 p1, undefined4 p2, float p3);
/*00424190*/ extern undefined debris_slot_clear(undefined4 p0);
/*004246f0*/ extern int car_set_damage();
/*00424ab0*/ extern int rwp_world_current_deactivate();
/*00424db0*/ extern int debris_vehicle_destroy_all();
/*004266c0*/ extern int race_events_process_log();
/*00426bb0*/ extern int ai_editor_paint_input();
/*00426de0*/ extern int ai_editor_cursor_move();
/*00426e90*/ extern int ai_editor_brush_select();
/*004271e0*/ extern int ai_editor();
/*004276b0*/ extern int __cdecl player_stat_get_crowns(undefined4 p0);
/*004276d0*/ extern int __cdecl player_stat_get_pants(undefined4 p0);
/*004276f0*/ extern int __cdecl player_stat_get_crowns_total(undefined4 p0);
/*00427710*/ extern int __cdecl player_stat_get_pants_total(undefined4 p0);
/*00428a20*/ extern int cars_step_race_phase_and_respawn();
/*00428e30*/ extern undefined __cdecl player_stats_update(undefined4 p0, undefined4 p1, undefined4 p2, undefined4 p3, undefined4 p4, undefined4 p5);
/*00434a10*/ extern int is_team_mode();
/*00434bb0*/ extern int get_event_type();
/*00467890*/ extern int player_status_set();
/*0047a640*/ extern int car_deactivate();
/*0047a810*/ extern int car_set_controls_disabled();
/*0047f300*/ extern int cars_step();
/*00481fd0*/ extern int random_range();
/*004a8420*/ extern int debug_printf();
/*004b3430*/ extern int __chkstk();
/*004d4490*/ extern RwMatrix * RwMatrixInvert(RwMatrix *p0, RwMatrix *p1);
/*004d5670*/ extern struct RwV3d * RwV3dTransformPoints(struct RwV3d *p0, struct RwV3d *p1, RwInt32 p2, RwMatrix *p3);
/*004dc920*/ extern void * RwIm3DTransform(RwIm3DVertex *p0, RwUInt32 p1, RwMatrix *p2, RwUInt32 p3);
/*004dc9f0*/ extern RwBool RwIm3DEnd();
/*004dcb80*/ extern RwBool RwIm3DRenderPrimitive(enum RwPrimitiveType p0);
/*004f6810*/ extern struct RpClump * RpClumpForAllAtomics(struct RpClump *p0, RpAtomicCallBack p1, void *p2);
/*005dd330*/ extern float float_1_0;
/*005dd338*/ extern float k_f_0_01;
/*005ddc28*/ extern float k_f_0;
/*00609008*/ extern undefined g_race_setup_609008;
/*006090d8*/ extern undefined g_race_setup_6090d8;
/*006091f8*/ extern undefined g_race_setup_6091f8;
/*006092c8*/ extern undefined g_race_setup_6092c8;
/*00609398*/ extern undefined g_race_setup_609398;
/*00609468*/ extern undefined g_race_setup_609468;
/*0065a9e8*/ extern undefined4 car_damage;
/*0065c0d0*/ extern undefined g_game_65c0d0;
/*006605b4*/ extern undefined g_vehicle_render_6605b4;
/*006614a0*/ extern undefined g_game_6614a0;
/*006614d8*/ extern undefined g_game_6614d8;
/*006614d9*/ extern undefined g_vehicle_render_6614d9;
/*006614da*/ extern undefined g_vehicle_render_6614da;
/*006614db*/ extern undefined g_vehicle_render_6614db;
/*006614e0*/ extern undefined g_vehicle_render_6614e0;
/*006614e4*/ extern undefined g_game_6614e4;
/*006614ec*/ extern undefined g_vehicle_render_6614ec;
/*007d2128*/ extern struct RwGlobalsRelease *RwEngineInstance;
/*007eebc0*/ extern struct GameState g_game_state;
/*007ef7b8*/ extern undefined4 game_mode;
/*007f01fc*/ extern struct PlayerSlot g_player_slots[4];
/*007f023c*/ extern undefined4 g_ai_editor_mode;
/*007f0258*/ extern undefined4 g_ai_editor_step_flag_a;
/*007f025c*/ extern undefined4 g_ai_editor_step_flag_b;
/*007f0260*/ extern undefined4 g_ai_editor_step_counter;
/*00801b0c*/ extern undefined4 g_car_801b0c;
/*008c88a0*/ extern undefined4 g_game_8c88a0;
/*008c88a4*/ extern undefined4 g_game_8c88a4;
/*008c88f0*/ extern undefined4 g_game_8c88f0;
/*008c88f4*/ extern undefined4 g_game_8c88f4;
/*008c8940*/ extern undefined4 g_vehicle_slots_8c8940;
/*008c8944*/ extern undefined4 g_game_8c8944;
/*008c8990*/ extern undefined4 g_vehicle_slots_8c8990;
/*008c8994*/ extern undefined4 g_game_8c8994;
/*008c89e0*/ extern undefined4 g_vehicle_slots_8c89e0;
/*008c89e4*/ extern undefined4 g_game_8c89e4;
/*008c8a30*/ extern undefined4 g_vehicle_slots_8c8a30;
/*008c8a34*/ extern undefined4 g_game_8c8a34;
/*008c8a80*/ extern undefined4 g_vehicle_slots_8c8a80;
/*008c8a84*/ extern undefined4 g_game_8c8a84;
/*008c8ad0*/ extern undefined4 g_vehicle_slots_8c8ad0;
/*008c8ad4*/ extern undefined4 g_game_8c8ad4;
/*008c8ae0*/ extern struct PlayerStatRecord g_player_stats_race[4];
/*008c8fc0*/ extern struct PlayerStatRecord g_player_stats_result[4];
/*008c94a0*/ extern struct PlayerStatRecord g_player_stats_total[4];
/*008d8b30*/ extern undefined4 player_count;
/* car_damage_at_local_point */

/* "(car, amount, local position): damage to the car part nearest the point (inferred;
   damage_add_clamped is the clamp at 50.0)" */

extern void __cdecl car_damage_at_local_point (char *param_1,char *param_2,char *param_3,char *param_4,char *param_5, char *param_6,char *param_7,char *param_8,char *param_9, char *param_10,char *param_11);


/* car_damage_at_world_point */

/* "(car, amount, world position): transforms the point into car space with the inverse matrix and
   calls car_damage_at_local_point" */

extern void car_damage_at_world_point(char *param_1,char *param_2,RwV3d *pointsIn);


/* debris_slot_clear_forward */

/* "calls debris_slot_clear" */

extern void debris_slot_clear_forward(char *param_1);


/* debris_slot_list_update_visible */

/* "for the 10 slots (stride 0x18) with a clump calls game_fn_424ab0" */

extern void __cdecl debris_slot_list_update_visible(char *param_1);


/* debris_any_active_all_vehicles */

/* "loops vehicle debris records calling vehicles_any_active_sub_424db0" */

extern void debris_any_active_all_vehicles(void);


/* vehicle_tyre_marks_render */

/* WARNING: Function: __chkstk replaced with injection: alloca_probe */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* "builds an Im3D vertex list from the tyre mark segments (0xF54 byte records) and renders them" */

extern void __cdecl vehicle_tyre_marks_render(char *param_1,char *param_2);


/* player_stat_add_turbo_used */

/* "(player, amount): g_player_stats_race[player].turbo_used += amount (called when the turbo charge
   is spent)" */

extern void __cdecl player_stat_add_turbo_used(char *param_1,char *param_2);


/* car_kill */

/* "(car): car_init, inactive, damage set to 50.0 on both sides, weapon status 2, no HUD in game
   mode 7" */

extern undefined4 car_kill(char *param_1);


/* ai_editor_update */

/* "runs the three AI editor input handlers when player slot 3 is the editor slot" */

extern void ai_editor_update(void);


/* player_stat_get_turbo_used_team */

/* "kind 2 'Turbo Skills': result table +0xFC, summed over the team" */

/*
 * Returns the 'Turbo Skills' value (turbo_used of the result table) for one player.
 * player_index: 0-3, index into g_player_stats_result / g_player_slots.
 * Solo: that player's own turbo_used. Team mode: summed over every player whose slot_state
 * matches this player's (inferred: same team), this player counted once.
 */
/* TIER2: unvalidated 2026-10-17 authority=diffemu */
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


/* player_stat_get_aggression_team */

/* "result table aggression, summed over the team in team mode (kind 3, 'Aggression')" */

/*
 * Returns the 'Aggression' statistic from the race results for one player.
 * player_index: 0-3, index into g_player_stats_result / g_player_slots.
 * Solo: that player's own aggression. Team mode: summed over every player whose slot_state
 * matches this player's (inferred: same team), this player counted once.
 */
/* TIER2: unvalidated 2026-10-17 authority=diffemu */
int player_stat_get_aggression_team(int player_index_in)
{
  int player_index = player_index_in;
  int slot_state;
  int aggression_team;

  /* is_team_mode() result first (0 = solo), then overwritten with the aggression value */
  if (is_team_mode() != 0) {
    slot_state = g_player_slots[player_index].state;
    aggression_team = g_player_stats_result[player_index].aggression;
    if ((g_player_slots[0].state == slot_state) && (player_index != 0)) {
      aggression_team = aggression_team + g_player_stats_result[0].aggression;
    }
    if ((g_player_slots[1].state == slot_state) && (player_index != 1)) {
      aggression_team = aggression_team + g_player_stats_result[1].aggression;
    }
    if ((g_player_slots[2].state == slot_state) && (player_index != 2)) {
      aggression_team = aggression_team + g_player_stats_result[2].aggression;
    }
    if ((g_player_slots[3].state == slot_state) && (player_index != 3)) {
      aggression_team = aggression_team + g_player_stats_result[3].aggression;
    }
    return aggression_team;
  }
  return g_player_stats_result[player_index].aggression;
}


/* player_stat_get_kills */

/* "result table kills, summed over the team in team mode (kind 4, 'Kills')" */

/*
 * Returns the kills statistic shown in the race results for one player.
 * player_index: 0-3, index into g_player_stats_result / g_player_slots.
 * Returns 0 for event type 4 (inferred: no kill stats in that event).
 * In team mode, adds the kills of every other player whose slot state
 * matches this player's (inferred: same team).
 */
/* TIER2: unvalidated 2026-10-16 authority=diffemu */
int __cdecl player_stat_get_kills(int player_index)
{
  int slot_state;
  int total_kills;

  total_kills = get_event_type();
  if (total_kills == 4) {
    return 0;
  }
  total_kills = is_team_mode();
  if (total_kills != 0) {
    slot_state = g_player_slots[player_index].state;
    total_kills = g_player_stats_result[player_index].kills;
    if ((g_player_slots[0].state == slot_state) && (player_index != 0)) {
      total_kills = total_kills + g_player_stats_result[0].kills;
    }
    if ((g_player_slots[1].state == slot_state) && (player_index != 1)) {
      total_kills = total_kills + g_player_stats_result[1].kills;
    }
    if ((g_player_slots[2].state == slot_state) && (player_index != 2)) {
      total_kills = total_kills + g_player_stats_result[2].kills;
    }
    if ((g_player_slots[3].state == slot_state) && (player_index != 3)) {
      total_kills = total_kills + g_player_stats_result[3].kills;
    }
  }
  else {
    total_kills = g_player_stats_result[player_index].kills;
  }
  return total_kills;
}


/* player_stat_get_wall_hits_team */

/* "result table wall_hits, summed over the team (kind 5, 'Cleanest Drive', lower is better)" */

/*
 * Returns the 'Cleanest Drive' statistic (wall_hits of the result table) for one player.
 * player_index: 0-3, index into g_player_stats_result / g_player_slots.
 * Solo: that player's own wall_hits. Team mode: summed over every player whose slot state
 * matches this player's (inferred: same team), this player counted once.
 * Lower is better.
 */
/* TIER2: unvalidated 2026-10-17 authority=diffemu */
int player_stat_get_wall_hits_team(int player_index)
{
  int slot_state;
  int team_wall_hits;
  
  /* is_team_mode() result first (0 = solo), then overwritten with the wall_hits value */
  if (is_team_mode() != 0) {
    slot_state = g_player_slots[player_index].state;
    team_wall_hits = g_player_stats_result[player_index].wall_hits;
    if ((g_player_slots[0].state == slot_state) && (player_index != 0)) {
      team_wall_hits = team_wall_hits + g_player_stats_result[0].wall_hits;
    }
    if ((g_player_slots[1].state == slot_state) && (player_index != 1)) {
      team_wall_hits = team_wall_hits + g_player_stats_result[1].wall_hits;
    }
    if ((g_player_slots[2].state == slot_state) && (player_index != 2)) {
      team_wall_hits = team_wall_hits + g_player_stats_result[2].wall_hits;
    }
    if ((g_player_slots[3].state == slot_state) && (player_index != 3)) {
      team_wall_hits = team_wall_hits + g_player_stats_result[3].wall_hits;
    }
    return team_wall_hits;
  }
  return g_player_stats_result[player_index].wall_hits;
}


/* event type whose race results carry no kill-type statistics (inferred from the kills family) */
#define EVENT_TYPE_NO_KILL_STATS (4)


/* player_stat_get_unforced_errors */

/* "result table unforced errors, summed over the team (kind 6, 'Unforced Errors')" */

/*
 * Returns the 'Unforced Errors' statistic shown in the race results for one player.
 * player_index: 0-3, index into g_player_stats_result / g_player_slots.
 * Returns 0 for event type EVENT_TYPE_NO_KILL_STATS (inferred: no such stats in that event).
 * Solo: that player's own unforced_errors. Team mode: summed over every player whose slot state
 * matches this player's (inferred: same team), this player counted once.
 */
/* TIER2: unvalidated 2026-10-17 authority=diffemu */
int __cdecl player_stat_get_unforced_errors(int player_index)
{
  int slot_state;
  int result;
  
  /* event type EVENT_TYPE_NO_KILL_STATS: no kill-type stats (inferred) */
  result = get_event_type();
  if (result == EVENT_TYPE_NO_KILL_STATS) {
    return 0;
  }
  /* solo: this player's own value only; the is_team_mode() result is overwritten below */
  result = is_team_mode();
  if (result != 0) {
    slot_state = g_player_slots[player_index].state;
    result = g_player_stats_result[player_index].unforced_errors;
    if ((g_player_slots[0].state == slot_state) && (player_index != 0)) {
      result = result + g_player_stats_result[0].unforced_errors;
    }
    if ((g_player_slots[1].state == slot_state) && (player_index != 1)) {
      result = result + g_player_stats_result[1].unforced_errors;
    }
    if ((g_player_slots[2].state == slot_state) && (player_index != 2)) {
      result = result + g_player_stats_result[2].unforced_errors;
    }
    if ((g_player_slots[3].state == slot_state) && (player_index != 3)) {
      result = result + g_player_stats_result[3].unforced_errors;
    }
  }
  else {
    result = g_player_stats_result[player_index].unforced_errors;
  }
  return result;
}


/* player_stat_get_turbo_used_total_team */

/* "total table +0xFC (team sum, 'Turbo Skills Overall')" */

/*
 * Returns the 'Turbo Skills Overall' value (turbo_used of the total table) for one player.
 * player_index: 0-3, index into g_player_stats_total / g_player_slots.
 * Solo: that player's own total. Team mode: summed over every player whose slot_state
 * matches this player's (inferred: same team), this player counted once.
 */
/* TIER2: unvalidated 2026-10-16 authority=diffemu */
int player_stat_get_turbo_used_total_team(int player_index)
{
  int slot_state;
  int turbo_used_total;

  /* is_team_mode() result first (0 = solo), then overwritten with the total */
  if (is_team_mode() != 0) {
    slot_state = g_player_slots[player_index].state;
    turbo_used_total = g_player_stats_total[player_index].turbo_used;
    if ((g_player_slots[0].state == slot_state) && (player_index != 0)) {
      turbo_used_total = turbo_used_total + g_player_stats_total[0].turbo_used;
    }
    if ((g_player_slots[1].state == slot_state) && (player_index != 1)) {
      turbo_used_total = turbo_used_total + g_player_stats_total[1].turbo_used;
    }
    if ((g_player_slots[2].state == slot_state) && (player_index != 2)) {
      turbo_used_total = turbo_used_total + g_player_stats_total[2].turbo_used;
    }
    if ((g_player_slots[3].state == slot_state) && (player_index != 3)) {
      turbo_used_total = turbo_used_total + g_player_stats_total[3].turbo_used;
    }
    return turbo_used_total;
  }
  return g_player_stats_total[player_index].turbo_used;
}


/* player_stat_get_aggression_total_team */

/* "total table aggression (team sum, 'Aggression Overall')" */

/* (player_index): returns the 'Aggression Overall' value shown for one player in the total table.
   Outside team mode it is that player's own total aggression. In team mode it is the sum over the
   player and every other player slot with the same team state (slot_state). */

/* TIER2: unvalidated 2026-10-17 authority=diffemu */
int player_stat_get_aggression_total_team(int player_index)
{
  int slot_state;
  int aggression_total;

  aggression_total = is_team_mode();
  if (aggression_total != 0) {
    /* team mode: start with this player's own total, then add each teammate
       (same slot state, not this player). Slot 3 returns early; the others fall through. (inferred) */
    slot_state = g_player_slots[player_index].state;
    aggression_total = g_player_stats_total[player_index].aggression;
    if ((g_player_slots[0].state == slot_state) && (player_index != 0)) {
      aggression_total = aggression_total + g_player_stats_total[0].aggression;
    }
    if ((g_player_slots[1].state == slot_state) && (player_index != 1)) {
      aggression_total = aggression_total + g_player_stats_total[1].aggression;
    }
    if ((g_player_slots[2].state == slot_state) && (player_index != 2)) {
      aggression_total = aggression_total + g_player_stats_total[2].aggression;
    }
    if ((g_player_slots[3].state == slot_state) && (player_index != 3)) {
      aggression_total = aggression_total + g_player_stats_total[3].aggression;
    }
  }
  else {
    aggression_total = g_player_stats_total[player_index].aggression;
  }
  return aggression_total;
}


/* player_stat_get_kills_total */

/* "total table kills ('Kills Overall')" */

/*
 * Returns the 'Kills Overall' statistic (kills of the total table) for one player.
 * player_index: 0-3, index into g_player_stats_total / g_player_slots.
 * Returns 0 for event type EVENT_TYPE_NO_KILL_STATS (inferred: no kill stats in that event).
 * Solo: that player's own total kills. Team mode: summed over every player whose slot state
 * matches this player's (inferred: same team), this player counted once.
 */
/* TIER2: unvalidated 2026-10-17 authority=diffemu */
int __cdecl player_stat_get_kills_total(int player_index)
{
  int slot_state;
  int total_kills;

  /* event type EVENT_TYPE_NO_KILL_STATS: no kill stats (inferred) */
  total_kills = get_event_type();
  if (total_kills == EVENT_TYPE_NO_KILL_STATS) {
    return 0;
  }
  /* solo: this player's own total kills only; the is_team_mode() result is overwritten below */
  if (is_team_mode() != 0) {
    slot_state = g_player_slots[player_index].state;
    total_kills = g_player_stats_total[player_index].kills;
    if ((g_player_slots[0].state == slot_state) && (player_index != 0)) {
      total_kills = total_kills + g_player_stats_total[0].kills;
    }
    if ((g_player_slots[1].state == slot_state) && (player_index != 1)) {
      total_kills = total_kills + g_player_stats_total[1].kills;
    }
    if ((g_player_slots[2].state == slot_state) && (player_index != 2)) {
      total_kills = total_kills + g_player_stats_total[2].kills;
    }
    if ((g_player_slots[3].state == slot_state) && (player_index != 3)) {
      total_kills = total_kills + g_player_stats_total[3].kills;
    }
    return total_kills;
  }
  return g_player_stats_total[player_index].kills;
}


/* player_stat_get_wall_hits_total_team */

/* "total table wall_hits ('Cleanest Drive Overall')" */

/*
 * Returns the 'Cleanest Drive Overall' value (wall_hits of the total table, lower is better) for one player.
 * player_index: 0-3, index into g_player_stats_total / g_player_slots.
 * Solo: that player's own total. Team mode: summed over every player whose slot_state
 * matches this player's (inferred: same team), this player counted once.
 */
/* TIER2: unvalidated 2026-10-16 authority=diffemu */
int player_stat_get_wall_hits_total_team(int player_index)
{
  int slot_state;
  int wall_hits_total;

  /* is_team_mode() result first (0 = solo), then overwritten with the total */
  wall_hits_total = is_team_mode();
  if (wall_hits_total != 0) {
    slot_state = g_player_slots[player_index].state;
    wall_hits_total = g_player_stats_total[player_index].wall_hits;
    if ((g_player_slots[0].state == slot_state) && (player_index != 0)) {
      wall_hits_total = wall_hits_total + g_player_stats_total[0].wall_hits;
    }
    if ((g_player_slots[1].state == slot_state) && (player_index != 1)) {
      wall_hits_total = wall_hits_total + g_player_stats_total[1].wall_hits;
    }
    if ((g_player_slots[2].state == slot_state) && (player_index != 2)) {
      wall_hits_total = wall_hits_total + g_player_stats_total[2].wall_hits;
    }
    if ((g_player_slots[3].state == slot_state) && (player_index != 3)) {
      wall_hits_total = wall_hits_total + g_player_stats_total[3].wall_hits;
    }
  }
  else {
    wall_hits_total = g_player_stats_total[player_index].wall_hits;
  }
  return wall_hits_total;
}


/* player_stat_get_unforced_errors_total */

/* "total table unforced errors ('Unforced Errors Overall')" */

/*
 * Returns the 'Unforced Errors Overall' value (unforced_errors of the total table) for one player.
 * player_index: 0-3, index into g_player_stats_total / g_player_slots.
 * Returns 0 for event type EVENT_TYPE_NO_KILL_STATS (inferred: no such stats in that event).
 * Solo: that player's own total. Team mode: summed over every player whose slot state
 * matches this player's (inferred: same team), this player counted once.
 */
/* TIER2: unvalidated 2026-10-17 authority=diffemu */
int __cdecl player_stat_get_unforced_errors_total(int player_index)
{
  int slot_state;
  int total_unforced_errors;
  
  total_unforced_errors = get_event_type();
  if (total_unforced_errors == EVENT_TYPE_NO_KILL_STATS) {
    return 0;
  }
  if (is_team_mode() != 0) {
    slot_state = g_player_slots[player_index].state;
    total_unforced_errors = g_player_stats_total[player_index].unforced_errors;
    if ((g_player_slots[0].state == slot_state) && (player_index != 0)) {
      total_unforced_errors = total_unforced_errors + g_player_stats_total[0].unforced_errors;
    }
    if ((g_player_slots[1].state == slot_state) && (player_index != 1)) {
      total_unforced_errors = total_unforced_errors + g_player_stats_total[1].unforced_errors;
    }
    if ((g_player_slots[2].state == slot_state) && (player_index != 2)) {
      total_unforced_errors = total_unforced_errors + g_player_stats_total[2].unforced_errors;
    }
    if ((g_player_slots[3].state == slot_state) && (player_index != 3)) {
      total_unforced_errors = total_unforced_errors + g_player_stats_total[3].unforced_errors;
    }
    return total_unforced_errors;
  }
  return g_player_stats_total[player_index].unforced_errors;
}


/* player_stat_get_place_team */

/* "result table place (team sum over controllers in use)" */

extern int player_stat_get_place_team(char *param_1);


/* game_get_game_8c8ae0 */

/* "return g_game_8c8ae0;" */

extern int game_get_game_8c8ae0(void);


/* player_stat_get_team_l */

/* "0x8C94A0" */

extern int player_stat_get_team_l(char *param_1);


/* player_stat_get_by_kind */

/* "(player, kind): switch over the statistic kinds" */

extern void __cdecl player_stat_get_by_kind(int *param_1,char *param_2,char *param_3);


/* player_stats_copy_to_totals */

/* "copies the per race statistic tables into the totals (0x8C90B4 / 0x8C9060 ...)" */

extern void player_stats_copy_to_totals(void);


/* player_stats_accumulate */

/* "adds the per race values to the running totals (0x8C9540 / 0x8C9594 ...)" */

extern void player_stats_accumulate(void);


/* race_update_cars_and_stats */

/* "ai_editor_update, race_mark_ai_slots, cars_step, race_events_process_log, player_stats_update"
    */

extern undefined4 __cdecl race_update_cars_and_stats(char *param_1);


/* race_setup_struct_1 */

/*  */

extern undefined * race_setup_struct_1(void);


/* race_setup_struct_0 */

/*  */

extern undefined * race_setup_struct_0(void);


/* race_setup_struct_2 */

/*  */

extern undefined * race_setup_struct_2(void);


/* race_setup_struct_3 */

/*  */

extern undefined * race_setup_struct_3(void);


/* race_setup_struct_4 */

/*  */

extern undefined * race_setup_struct_4(void);


/* race_setup_struct_5 */

/*  */

extern undefined * race_setup_struct_5(void);


/* best_times_init_defaults */

/* fills the 3 x 45 best time tables (9 min 59.xx s) */

extern void best_times_init_defaults(void);


/* vehicle_slots_clear */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* "clears 8 records of 0x50 bytes at 0x8C88A0" */

extern void vehicle_slots_clear(void);


/* vehicle_slots_clear_flags */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/*
 * Sets the eight vehicle slot / game flag globals (g_game_8c88a0 .. g_vehicle_slots_8c8ad0) to 0.
 * No parameters, no return value (purpose inferred from the name and the body).
 */

/* TIER2: validated 2026-10-18 authority=diffemu */
void vehicle_slots_clear_flags(void)
{
  g_game_8c88a0 = 0;
  g_game_8c88f0 = 0;
  g_vehicle_slots_8c8940 = 0;
  g_vehicle_slots_8c8990 = 0;
  g_vehicle_slots_8c89e0 = 0;
  g_vehicle_slots_8c8a30 = 0;
  g_vehicle_slots_8c8a80 = 0;
  g_vehicle_slots_8c8ad0 = 0;
  return;
}

/* byte offsets of the three atomic slots in the table at lookup_state[1] (inferred) */
#define ATOMIC_SLOT0_OFFSET (4)
#define ATOMIC_SLOT1_OFFSET (8)
#define ATOMIC_SLOT2_OFFSET (0xc)


/* atomic_lookup_error */

/* "No such atomic as %d" */

/* TIER2: unvalidated 2026-10-11 authority=diffemu */
/*
 * RpAtomicCallBack for RpClumpForAllAtomics (clump walk order).
 * atomic: the RpAtomic being visited.
 * lookup_state: int[2]; [0] = index of this atomic in the walk (count of atomics seen so far),
 *               [1] = pointer to the slot table (inferred).
 * Stores atomic in slot 0, 1 or 2 for indices 0, 1, 2; any other index is logged and dropped.
 * Always advances the counter and returns atomic, so the walk continues (inferred).
 */
/* TIER2: validated 2026-10-18 authority=diffemu */
undefined4 atomic_lookup_error(undefined4 atomic,int *lookup_state)
{
  switch (*lookup_state) {
  case 0:
    *(undefined4 *)(lookup_state[1] + ATOMIC_SLOT0_OFFSET) = atomic;
    break;
  case 1:
    *(undefined4 *)(lookup_state[1] + ATOMIC_SLOT1_OFFSET) = atomic;
    break;
  case 2:
    *(undefined4 *)(lookup_state[1] + ATOMIC_SLOT2_OFFSET) = atomic;
    break;
  default:
    debug_printf("No such atomic as %d\n",*lookup_state);
    break;
  }
  *lookup_state = *lookup_state + 1;
  return atomic;
}


/* clump_check_atomics_lookup */

/* "RpClumpForAllAtomics(clump, atomic_lookup_error)" */

extern void clump_check_atomics_lookup(undefined4 *param_1);
#undef local_8
#undef local_4



