#include "mfl_tier2.h"
/*00401000*/ extern int load_cup_model();
/*00405e40*/ extern int stub_return_zero();
/*0040aeb0*/ extern int gamesave_request_save_a();
/*0040aef0*/ extern int gamesave_request_delete();
/*0040af20*/ extern int autosave_request();
/*0040d0d0*/ extern int __cdecl find_texture_dict_b(RwChar *p0);
/*0040fdb0*/ extern int get_player_count();
/*00416200*/ extern int trail_particles_create();
/*0041a380*/ extern int ai_ftol_noop_wrapper();
/*004241e0*/ extern int car_damage_smoke_create();
/*0042b0d0*/ extern int course_get_safe_start_count();
/*0042b0e0*/ extern int course_get_lap_line();
/*0042f860*/ extern int __cdecl texdict_load_named_b(char *p0);
/*004316f0*/ extern int message_close();
/*00434a10*/ extern int is_team_mode();
/*0047a5b0*/ extern int car_add_velocity();
/*0047b520*/ extern int car_current_matrix();
/*0047b6e0*/ extern int car_get_velocity();
/*00485560*/ extern int particle_pools_init();
/*004855b0*/ extern undefined __cdecl particle_pool_add(struct RwTexture *p0, uint p1);
/*004860c0*/ extern int smoke_particles_init();
/*00487480*/ extern int shockwave_load_models();
/*00487ec0*/ extern int effect_pool_init();
/*004983b0*/ extern int fx_spritecomp_create();
/*004988e0*/ extern int fx_explosion_systems_init();
/*00499ae0*/ extern int load_scorch_decal();
/*0049a940*/ extern undefined __cdecl fx_dust_create_default(undefined4 p0, undefined4 p1, undefined4 p2);
/*0049b750*/ extern int fx_slots_clear();
/*0049c920*/ extern int fx_objects_clear();
/*0049d1e0*/ extern int load_waterfall_fx();
/*0049dfc0*/ extern int fx_texture_load();
/*0049fe00*/ extern int tri_model_load();
/*004a43d0*/ extern undefined __cdecl game_open_piz(char *p0);
/*004a4440*/ extern int game_close_piz();
/*004b2bbd*/ extern int __cdecl FID_conflict__wprintf(char *p0, ...);
/*004b2ccd*/ extern undefined __cdecl start(undefined4 p0, undefined4 p1, undefined4 p2, undefined4 p3, undefined4 p4);
/*004b322b*/ extern int exit();
/*004d43f0*/ extern RwMatrix * RwMatrixRotate(RwMatrix *p0, struct RwV3d *p1, RwReal p2, enum RwOpCombineType p3);
/*004d4870*/ extern RwMatrix * RwMatrixTranslate(RwMatrix *p0, struct RwV3d *p1, enum RwOpCombineType p2);
/*004d54d0*/ extern RwReal RwV2dLength(struct RwV2d *p0);
/*004d5e10*/ extern struct RwTexDictionary * RwTexDictionarySetCurrent(struct RwTexDictionary *p0);
/*0050b5e0*/ extern int nop_stub();
/*005dd330*/ extern float float_1_0;
/*005dd364*/ extern float k_f_n1;
/*005ddacc*/ extern float k_f_360;
/*005ddafc*/ extern float k_f_0_05;
/*005ddb10*/ extern float k_f_0_99;
/*005ddc28*/ extern float k_f_0;
/*00651ac0*/ extern undefined4 g_line_exceptions;
/*00652d04*/ extern undefined4 g_nop_a_652d04;
/*00652d08*/ extern undefined g_course_update_652d08;
/*00652d0c*/ extern undefined g_course_update_652d0c;
/*00652d10*/ extern undefined g_course_update_652d10;
/*00652d14*/ extern undefined g_course_update_652d14;
/*00652d18*/ extern undefined g_course_update_652d18;
/*00652d1c*/ extern undefined g_course_update_652d1c;
/*00652d20*/ extern undefined g_course_update_652d20;
/*00652d24*/ extern undefined g_course_update_652d24;
/*00652d28*/ extern undefined g_course_update_652d28;
/*00652d2c*/ extern undefined g_course_update_652d2c;
/*00652dbc*/ extern undefined4 g_round_652dbc;
/*00652dd0*/ extern undefined4 g_effects_652dd0;
/*00652dd4*/ extern undefined4 g_effects_652dd4;
/*00652dd8*/ extern undefined4 g_load_652dd8;
/*00652ddc*/ extern undefined4 g_effects_652ddc;
/*00652de0*/ extern undefined4 g_effects_652de0;
/*00652de4*/ extern undefined4 g_effects_652de4;
/*007ef7b0*/ extern undefined4 g_car_7ef7b0;
/*007ef7b8*/ extern undefined4 game_mode;
/*007f01fc*/ extern struct PlayerSlot g_player_slots[4];
/*008bd7a0*/ extern undefined4 g_rank_order;
/*008d8b40*/ extern undefined4 player_points;
/*008d8b44*/ extern undefined4 g_round_8d8b44;
/*008d8b48*/ extern undefined4 g_round_8d8b48;
/*008d8b4c*/ extern undefined4 g_round_8d8b4c;
/*008d8b50*/ extern undefined4 g_player_8d8b50;
/*008d8b54*/ extern undefined4 g_round_8d8b54;
/*008d8b58*/ extern undefined4 g_round_stats_8d8b58;
/*008d8b5c*/ extern undefined4 g_round_8d8b5c;
/*008d8b60*/ extern undefined4 g_round_8d8b60;
/*008d8b64*/ extern undefined4 g_round_8d8b64;
/*008d8b68*/ extern undefined4 g_round_8d8b68;
/*008d8b6c*/ extern undefined4 g_round_8d8b6c;
/*008d8b70*/ extern undefined4 g_round_8d8b70;
/*008d8b74*/ extern undefined4 g_round_setup_8d8b74;
/*008d8b78*/ extern undefined4 g_round_setup_8d8b78;
/*008d8b7c*/ extern undefined4 g_round_setup_8d8b7c;
/*008d8b80*/ extern undefined4 g_round_8d8b80;
/*008d8b84*/ extern undefined4 g_round_8d8b84;
/*008d8b88*/ extern undefined4 g_round_8d8b88;
/*008d8b8c*/ extern undefined4 g_round_8d8b8c;
/*008d8b90*/ extern undefined4 g_player_8d8b90;
/*008d8b94*/ extern undefined4 g_round_8d8b94;
/*008d8b98*/ extern undefined4 g_round_stats_8d8b98;
/*008d8b9c*/ extern undefined4 g_round_stats_8d8b9c;
/*008d8ba0*/ extern undefined4 g_round_8d8ba0;
/*008d8ba4*/ extern undefined4 g_round_stats_8d8ba4;
/*008d8ba8*/ extern undefined4 g_round_stats_8d8ba8;
/*008d8bac*/ extern undefined4 g_round_stats_8d8bac;
/*008d8bb0*/ extern undefined4 g_round_8d8bb0;
/*008d8bb4*/ extern undefined4 g_round_stats_8d8bb4;
/*008d8bb8*/ extern undefined4 g_round_stats_8d8bb8;
/*008d8bbc*/ extern undefined4 g_round_stats_8d8bbc;
/*008d8bc0*/ extern undefined4 g_round_8d8bc0;
/*008d8bc4*/ extern undefined4 g_round_stats_8d8bc4;
/*008d8bc8*/ extern undefined4 g_round_stats_8d8bc8;
/*008d8bcc*/ extern undefined4 g_round_stats_8d8bcc;
/*008d8bd0*/ extern undefined4 g_round_8d8bd0;
/*008d8bd4*/ extern undefined4 g_round_setup_8d8bd4;
/*008d8bd8*/ extern undefined4 g_round_setup_8d8bd8;
/*008d8bdc*/ extern undefined4 g_round_setup_8d8bdc;
/*008d8be4*/ extern undefined4 g_gamesave_operation;
/*008d8be8*/ extern undefined4 g_gamesave_format_step;
/*008d8bec*/ extern undefined4 g_gamesave_load_step;
/*008d8bf0*/ extern undefined4 g_gamesave_save_step;
/*008d8bf4*/ extern undefined4 g_autosave_step;
/*008d8bf8*/ extern undefined4 g_gamesave_delete_step;
/*008d8c00*/ extern undefined4 g_gamesave_result_code;
/*008d8c0c*/ extern undefined4 g_gamesave_flag_c0c;
/*008d8ca0*/ extern undefined g_player_8d8ca0;
/*008d8ca4*/ extern undefined g_player_8d8ca4;
/*008d8d48*/ extern undefined g_player_8d8d48;
/*008d8d4c*/ extern undefined g_player_8d8d4c;
/*008d8d50*/ extern undefined g_player_8d8d50;
/*008d8d54*/ extern undefined g_player_8d8d54;
/* player_rank_key */

/* float used to sort the 4 players (g_rank_order) */

extern double player_rank_key(char *param_1);


/* player_set_precise_position */

/* "logs put precisepos and stores the integer into the per player record" */

extern undefined4 player_set_precise_position(char *param_1,float param_2);


/* player_progress_b */

/* "float at 0x8D8D4C + i * 0x30C" */

extern double player_progress_b(char *param_1);


/* course_lap_line_find_index */

/* "loops course_get_lap_line over the safe start count and returns the matching index" */

extern void course_lap_line_find_index(int *param_1);


/* gamesave_ptr_to_line_exceptions */

/* "return &g_line_exceptions;" */

extern undefined4 * gamesave_ptr_to_line_exceptions(void);


/* gamesave_set_result_code */

/* "stores g_gamesave_result_code" */

extern void gamesave_set_result_code(char *param_1);


/* gamesave_poll_status */

/* "Lua stub query then status change handling (message_close) for the save state machines" */

extern void gamesave_poll_status(void);


/* gamesave_cb_save_state */

/* "handler for the manual save state variable 0x8D8BF0" */

extern void gamesave_cb_save_state(void);


/* gamesave_cb_dispatch_a */

/* "switch on the active save operation 0x8D8BE4" */

extern void gamesave_cb_dispatch_a(void);


/* gamesave_cb_dispatch_b */

/*  */

extern void gamesave_cb_dispatch_b (char *param_1,char *param_2,char *param_3,char *param_4, char *param_5,char *param_6,char *param_7,char *param_8, char *param_9,char *param_10,char *param_11,char *param_12, char *param_13,char *param_14,char *param_15,char *param_16);


/* nop_a */

/*  */

extern undefined4 nop_a(void);


/* nop_b */

/*  */

extern undefined4 nop_b(void);


/* gamesave_set_autosave_enabled */

/* "0x8D8C0C = value (autosave_request only acts when it is set)" */

extern void __cdecl gamesave_set_autosave_enabled(char *param_1);


/* car_scale_velocity */

/* "(car, factor): reads the velocity, multiplies it and writes it back" */

extern void __cdecl car_scale_velocity(char *param_1,float param_2);
#undef local_c
#undef local_8
#undef local_4


/* car_push_from_matrix */

/* "(car, a, b): velocity change along the car's matrix axes scaled by 360 (inferred)" */

extern void __cdecl car_push_from_matrix(char *param_1,float param_2,float param_3);


/* car_apply_speed_boost_along_forward */

/* "adds a velocity along the car's forward vector scaled by 0.05 * horizontal speed" */

extern void __cdecl car_apply_speed_boost_along_forward(char *param_1,char *param_2);


/* car_wobble_tilt_update */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* "damped yaw/roll oscillation of the car matrix (tables at 0x652d08, stride 0x28) around the car
   origin" */

extern void __cdecl car_wobble_tilt_update(float angle);
extern char g_ai_editor_mode[];

#define POINT_LIMIT_DEFAULT (6)
#define POINT_LIMIT_REDUCED (4)
#define POINTS_CHANGE_INIT (0xfffffc18) /* -1000 */
#define CONTROLLER_NONE (-1) /* PlayerSlot.controller of a slot with no player */
#define STAT_ABSENT_PLAYER (-100) /* stat value sorted last for absent players */
#define PLAYER_SLOT_COUNT (4)
#define PLAYER_SLOTS_END (((LONG_PTR)&g_ai_editor_mode)) /* one past the end of g_player_slots */
#define POINTS_ABSENT_PLAYER (-100) /* points sort key of a slot with no player (sorted last) */


/* round_setup_limits */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* "sets the round point limit (6, or 4 for 2 / 3 players, team mode and modes 1 / 2) and the timer
   start -1000" */
/* Round setup: picks the point limit (POINT_LIMIT_DEFAULT, or POINT_LIMIT_REDUCED for 2 or 3 players,
   team mode, game modes 1 and 2), then writes that limit into the per-player round slots and resets
   the change tables to POINTS_CHANGE_INIT (-1000). The "timer start" wording above is a guess (inferred). */

/* TIER2: validated 2026-10-18 authority=diffemu */
void round_setup_limits(void)
{
  int count_or_flag;
  undefined4 point_limit;

  /* default limit, lowered below for the reduced cases */
  point_limit = POINT_LIMIT_DEFAULT;
  /* 2 players -> reduced limit */
  count_or_flag = get_player_count();
  if (count_or_flag == 2) {
    point_limit = POINT_LIMIT_REDUCED;
  }
  /* 3 players -> reduced limit */
  count_or_flag = get_player_count();
  if (count_or_flag == 3) {
    point_limit = POINT_LIMIT_REDUCED;
  }
  /* team mode -> reduced limit */
  count_or_flag = is_team_mode();
  if (count_or_flag != 0) {
    point_limit = POINT_LIMIT_REDUCED;
  }
  /* game modes 1 and 2 -> reduced limit */
  if ((game_mode == GAME_MODE_HOLD_THE_FLAG) || (game_mode == GAME_MODE_HARE_AND_HOUNDS)) {
    point_limit = POINT_LIMIT_REDUCED;
  }
  /* change tables (0x8D8B60 / 0x8D8B80 per the dossier) reset to -1000 */
  g_round_8d8b60 = POINTS_CHANGE_INIT;
  g_round_8d8b64 = POINTS_CHANGE_INIT;
  g_round_8d8b68 = POINTS_CHANGE_INIT;
  g_round_8d8b6c = POINTS_CHANGE_INIT;
  g_round_8d8b80 = POINTS_CHANGE_INIT;
  player_points = point_limit;
  g_round_8d8b70 = 0;
  g_round_8d8b84 = 0xfffffc18;
  g_round_8d8b44 = point_limit;
  g_round_setup_8d8b74 = 0;
  g_round_8d8b88 = 0xfffffc18;
  g_round_8d8b48 = point_limit;
  g_round_setup_8d8b78 = 0;
  g_round_8d8bd0 = point_limit;
  g_round_setup_8d8bd4 = point_limit;
  g_round_setup_8d8bd8 = point_limit;
  g_round_setup_8d8bdc = point_limit;
  g_round_8d8b8c = 0xfffffc18;
  g_round_8d8b4c = point_limit;
  g_round_setup_8d8b7c = 0;
  return;
}


extern void round_scores_reset(void);


extern undefined4 player_get_points_change(char *param_1);


extern undefined4 player_get_points_change_visual(char *param_1);


/* players_get_points */

/* Ranks the 4 player slots by player_points (inferred: the table at 0x8D8B40). out_order[k] receives
   the index of the player in rank k, best first. Absent players (controller == CONTROLLER_NONE) use
   POINTS_ABSENT_PLAYER and sort last. Equal points keep the lower player index first. Game modes 4 / 7
   overwrite out_order with ai_ftol_noop_wrapper(i) results, and game mode 9 swaps out_order[0] / out_order[1]
   depending on g_car_7ef7b0 (both inferred). Returns nothing. */
/* "(out[4]): player indices sorted by player_points, -100 for absent players (inferred)" */

/* TIER2: validated 2026-10-18 authority=diffemu */
void players_get_points(undefined4 *out_order)
{
  undefined4 *cur_out_ptr;
  int cur_points;
  undefined4 cur_idx;
  char *out_offset;
  int i;
  undefined4 next_idx;
  PlayerSlot *slot;
  int pos;
  int slot_points [4];

  /* fill slot_points[] from player_points for each slot (or POINTS_ABSENT_PLAYER) and seed out_order with 0..3;
     i is the player index here and the pass count in the sort below (inferred: reused variable) */
  i = 0;
  slot = g_player_slots;
  do {
    slot_points[i] = (&player_points)[i];
    if (slot->controller == CONTROLLER_NONE) {
      slot_points[i] = POINTS_ABSENT_PLAYER;
    }
    out_order[i] = i;
    slot = slot + 1;
    i = i + 1;
  } while ((LONG_PTR)slot < PLAYER_SLOTS_END);
  /* bubble sort, descending by points: PLAYER_SLOT_COUNT - 1 passes, each swap also moves the
     matching out_order entries */
  i = PLAYER_SLOT_COUNT - 1;
  do {
    pos = 0;
    if (0 < i) {
      do {
        /* swap the adjacent pair only when the next points value is strictly greater (equal keeps order) */
        cur_points = slot_points[pos];
        if (cur_points < slot_points[pos + 1]) {
          slot_points[pos] = slot_points[pos + 1];
          out_offset = pos * 4 - (int)slot_points;
          next_idx = *(undefined4 *)((LONG_PTR)out_order + (LONG_PTR)slot_points + out_offset + 4);
          cur_out_ptr = (undefined4 *)((LONG_PTR)out_order + (LONG_PTR)slot_points + out_offset);
          slot_points[pos + 1] = cur_points;
          cur_idx = *cur_out_ptr;
          *cur_out_ptr = next_idx;
          *(undefined4 *)((LONG_PTR)out_order + (LONG_PTR)slot_points + out_offset + 4) = cur_idx;
        }
        pos = pos + 1;
      } while (pos < i);
    }
    i = i + -1;
  } while (0 < i);
  /* game modes 4 and 7: overwrite every rank entry with ai_ftol_noop_wrapper(i) (inferred) */
  if ((game_mode == GAME_MODE_RACE) || (game_mode == GAME_MODE_AIRSTRIKE)) {
    i = 0;
    do {
      next_idx = ai_ftol_noop_wrapper(i);
      out_order[i] = next_idx;
      i = i + 1;
    } while (i < PLAYER_SLOT_COUNT);
  }
  /* game mode 9: the first two ranks depend on g_car_7ef7b0 (inferred) */
  if (game_mode == GAME_MODE_CHASE_FUGITIVE) {
    if (g_car_7ef7b0 != 0) {
      *out_order = 0;
      out_order[1] = 1;
      return;
    }
    *out_order = 1;
    out_order[1] = 0;
  }
  return;
}


/* players_get_stat_8b90 */

/* Ranks the players by the 0x8D8B90 stat table. out_player_order[k] receives the index of the player
   in rank k, best first (out_player_order is an int[4] output). Absent players (controller ==
   CONTROLLER_NONE) use STAT_ABSENT_PLAYER and sort last. Equal stats keep the lower player index first.
   Returns nothing. */

/* "(out[4]): 0x8D8B90 table, -100 for absent players" */

/* TIER2: unvalidated 2026-10-17 authority=diffemu */
void players_get_stat_8b90(char *out_player_order)
{
  char *out_delta;
  int cur_stat;
  undefined4 out_next;
  undefined4 out_cur;
  int idx;
  PlayerSlot *slot;
  int pos;
  int stats [PLAYER_SLOT_COUNT];

  /* fill stats[] from 0x8D8B90 for each slot (or STAT_ABSENT_PLAYER) and seed out_player_order with 0..3;
     idx is the player index here and the pass count in the sort below (inferred: reused variable) */
  idx = 0;
  slot = g_player_slots;
  do {
    stats[idx] = (&g_player_8d8b90)[idx];
    if (slot->controller == CONTROLLER_NONE) {
      stats[idx] = STAT_ABSENT_PLAYER;
    }
    *(int *)(out_player_order + idx * 4) = idx;
    slot = slot + 1;
    idx = idx + 1;
  } while ((LONG_PTR)slot < PLAYER_SLOTS_END);
  /* bubble sort, descending by stat: PLAYER_SLOT_COUNT - 1 passes, each swap also moves the
     matching out_player_order entries */
  idx = 3;
  do {
    pos = 0;
    if (0 < idx) {
      do {
        /* swap the adjacent pair only when the next stat is strictly greater (equal keeps order) */
        cur_stat = stats[pos];
        if (cur_stat < stats[pos + 1]) {
          stats[pos] = stats[pos + 1];
          out_delta = (out_player_order - (int)stats) + pos * 4;
          out_next = *(undefined4 *)((LONG_PTR)stats + out_delta + 4);
          stats[pos + 1] = cur_stat;
          out_cur = *(undefined4 *)((LONG_PTR)stats + out_delta);
          *(undefined4 *)((LONG_PTR)stats + out_delta) = out_next;
          *(undefined4 *)((LONG_PTR)stats + out_delta + 4) = out_cur;
        }
        pos = pos + 1;
      } while (pos < idx);
    }
    idx = idx + -1;
  } while (0 < idx);
  return;
}


/* player_get_stat_8b90 */

/* "0x8D8B90[player]" */

extern undefined4 player_get_stat_8b90(char *param_1);


/* player_get_wins */

/* "0x8D8B50[player]" */

extern undefined4 player_get_wins(char *param_1);


/* player_get_points */

/* "player_points[i]" */

extern undefined4 player_get_points(char *param_1);


/* placement_counter_increment */

/* "2 in team mode (unless game mode 2), else the player count" */

extern void placement_counter_increment(char *param_1,char *param_2);


/* round_get_round_652dbc */

/* "return g_round_652dbc;" */

extern undefined4 round_get_round_652dbc(void);


/* round_stats_reset */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* "clears the per player round statistics (0x8D8BA0 / 0x8D8BB0 tables)" */
/* Purpose (inferred): zeroes the round-wide and per-player round statistics globals
 * (g_round_*, g_round_stats_*, g_player_* at 0x8D8B50..0x8D8BCC, plus g_round_652dbc).
 * Takes no parameters and returns nothing. Called at the start of a new round. (inferred) */

/* TIER2: validated 2026-10-18 authority=diffemu */
void round_stats_reset(void)
{
  g_round_8d8bb0 = 0;
  g_round_8d8ba0 = 0;
  g_round_stats_8d8bb4 = 0;
  g_round_stats_8d8ba4 = 0;
  g_round_stats_8d8bb8 = 0;
  g_round_stats_8d8ba8 = 0;
  g_round_stats_8d8bbc = 0;
  g_round_stats_8d8bac = 0;
  g_round_8d8bc0 = 0;
  g_player_8d8b90 = 0;
  g_player_8d8b50 = 0;
  g_round_stats_8d8bc4 = 0;
  g_round_8d8b94 = 0;
  g_round_8d8b54 = 0;
  g_round_stats_8d8bc8 = 0;
  g_round_stats_8d8b98 = 0;
  g_round_stats_8d8b58 = 0;
  g_round_stats_8d8bcc = 0;
  g_round_stats_8d8b9c = 0;
  g_round_8d8b5c = 0;
  g_round_652dbc = 0;
  return;
}


/* players_reached_target */

/* "(out[4]): flag when player_points >= score_target_b" */

extern void players_reached_target(undefined4 *param_1);


/* race_load_common_assets */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* "opens sfx.piz, fx.txd; loads cup / tri / shockwave models" */

extern void race_load_common_assets(void);


