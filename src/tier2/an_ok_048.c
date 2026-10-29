#include "mfl_tier2.h"
/*00409f30*/ extern double player_progress_b();
/*0040fdc0*/ extern int race_get_phase();
/*0040ff00*/ extern int race_get_race_652dec();
/*0040ff20*/ extern int race_setup_get_slot_type();
/*0040ff70*/ extern int race_get_last_active_player();
/*0040ffc0*/ extern int round_find_winner();
/*00434bb0*/ extern int get_event_type();
/*00435f10*/ extern int get_career_row();
/*004377a0*/ extern int option_get_slider_4();
/*0044a4f0*/ extern int camera_get_position();
/*00467eb0*/ extern int __cdecl player_has_no_weapon(undefined4 p0);
/*004687f0*/ extern int sound_ducking_adjust_by_event();
/*00468a20*/ extern int __cdecl car_is_playable(undefined4 p0);
/*00468ac0*/ extern int voice_queue_add();
/*00468b80*/ extern int voice_queue_remove();
/*00468cd0*/ extern int voice_queue_update();
/*00469040*/ extern int voice_queue_remove_table_entries();
/*004692f0*/ extern int voice_queue_race_update();
/*0046a010*/ extern int sound_stop_all_voices_and_streams();
/*00471ff0*/ extern int sound_stream_start_if_idle();
/*004720c0*/ extern int voice_queue_play_pending();
/*00473450*/ extern int sound_stop_all_and_release();
/*004757a0*/ extern int get_viewport_camera_matrix();
/*00481ed0*/ extern int __cdecl table_get_6c3f38(undefined4 p0);
/*004821a0*/ extern int __cdecl vec3_is_valid_small(undefined4 p0);
/*00482220*/ extern int __cdecl matrix_is_valid_small(undefined4 p0, undefined4 p1);
/*004a5da0*/ extern int object_destroy_frames();
/*004b4ba7*/ extern int entry();
/*004d53a0*/ extern RwReal RwV3dLength(struct RwV3d *p0);
/*004f6a60*/ extern RwBool RpAtomicDestroy(struct RpAtomic *p0);
/*005569b0*/ extern struct RpSpline * RpSplineCreate(RwInt32 p0, RwInt32 p1, struct RwV3d *p2);
/*005dd330*/ extern float float_1_0;
/*005dd338*/ extern float k_f_0_01;
/*005dd340*/ extern float k_f_0_5;
/*005dd374*/ extern float k_f_0_2;
/*005dd410*/ extern float k_f_n2;
/*005dd620*/ extern float float_10_0;
/*005ddb5c*/ extern float k_f_n4;
/*005e64a4*/ extern float k_f_0;
/*005df93c*/ extern float k_f_63;
/*005df998*/ extern float k_f_n15;
/*00610dfc*/ extern undefined g_weapon_610dfc;
/*00611418*/ extern undefined g_sound_frame_611418;
/*006124f0*/ extern undefined g_sound_frame_6124f0;
/*006135c8*/ extern undefined g_sound_frame_6135c8;
/*006146a0*/ extern undefined g_sound_frame_6146a0;
/*00615778*/ extern undefined g_sound_frame_615778;
/*00616850*/ extern undefined g_sound_frame_616850;
/*00617af0*/ extern undefined4 g_sound_flag_617af0;
/*006aaf64*/ extern undefined4 g_car_6aaf64;
/*006aaf74*/ extern undefined4 g_weapon_6aaf74;
/*006ad1e4*/ extern undefined4 g_sound_6ad1e4;
/*006ad1e8*/ extern undefined4 g_sound_6ad1e8;
/*006ad1ec*/ extern undefined4 g_sound_frame_6ad1ec;
/*006ad1f0*/ extern undefined4 g_sound_frame_6ad1f0;
/*006ad1f4*/ extern undefined4 g_sound_frame_6ad1f4;
/*006ad200*/ extern float g_sound_frame_6ad200;
/*006ad204*/ extern float g_sound_frame_6ad204;
/*006ad208*/ extern float g_sound_frame_6ad208;
/*006ad8ac*/ extern float g_sound_music_fade;
/*006ae0b0*/ extern float g_sound_6ae0b0;
/*006ae0b4*/ extern float g_sound_6ae0b4;
/*006ae0b8*/ extern float g_sound_6ae0b8;
/*006ae24c*/ extern undefined4 g_course_unload_6ae24c;
/*006af57c*/ extern undefined g_sound_get_6af57c;
/*006af594*/ extern undefined4 g_course_unload_6af594;
/*007ef7b8*/ extern undefined4 game_mode;
/*007ef820*/ extern struct PlayerInput g_player_inputs[12];
/*007efce0*/ extern struct PlayerInput g_player_inputs_previous[12];
/*007f01fc*/ extern struct PlayerSlot g_player_slots[4];
/*008bd7bc*/ extern undefined4 g_weapon_ai_8bd7bc;
/*008bdcc0*/ extern undefined4 g_weapon_ai_8bdcc0;
/*008be1e0*/ extern undefined4 g_weapon_8be1e0;
/*008be1e4*/ extern undefined4 g_weapon_8be1e4;
/*008be1e8*/ extern undefined4 g_weapon_ai_8be1e8;
/*008be1ec*/ extern undefined4 g_weapon_ai_8be1ec;
/*008be1f0*/ extern undefined4 g_weapon_ai_8be1f0;
/*008be200*/ extern undefined g_weapon_8be200;
/*008be720*/ extern undefined4 g_weapon_8be720;
/*008be7c0*/ extern undefined4 g_sound_stream_slots;
/*008be7e0*/ extern char *g_sound_8be7e0;
/*008be7f0*/ extern undefined4 g_weapon_ai_8be7f0;
/*008bed00*/ extern undefined4 g_sound_frame_8bed00;
/*008d98bc*/ extern int g_sound_player_count;
/*008d9940*/ extern undefined4 g_sound_8d9940;
/*008d9944*/ extern undefined4 g_weapon_8d9944;
/*008d9948*/ extern undefined4 g_weapon_ai_8d9948;
/*008d994c*/ extern undefined4 g_weapon_ai_8d994c;
/*008d9960*/ extern undefined4 g_sound_8d9960;
/* course_surface_id_is_active_check */

/* "maps the surface colour code (0xff0N3cff/0xff0Nffff ...) to the table_get_6c3f38 flags" */

extern uint __cdecl course_surface_id_is_active_check(char *param_1);


/* player_has_weapon */

/* "nonzero when the player (car index) already carries a weapon, so a pick-up is refused" */

extern bool __cdecl player_has_weapon(char *param_1);

/* dword count of the g_weapon_8be720 table (6 per row, 6 rows) (inferred) */
#define WEAPON_8BE720_WORD_COUNT (0x24)


/* voice_queue_tables_clear */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* "clears the queue tables at 0x8BE1E0 / 0x8BE7F0 / 0x8BD7BC / 0x8BE1F0 / 0x8BDCC0 (was
   weapon_ai_tables_clear)" */

/* Resets the voice / weapon AI queue tables to 0: the scalar counters first, then the
   WEAPON_8BE720_WORD_COUNT dwords of g_weapon_8be720, then g_weapon_ai_8d994c. Takes no
   parameters and returns nothing. Called on queue reset (inferred). */
/* TIER2: validated 2026-10-18 authority=diffemu */
void voice_queue_tables_clear(void)
{
  int countdown;
  undefined4 *cursor;

  g_weapon_8be1e0 = 0;
  g_weapon_ai_8be7f0 = 0;
  g_weapon_ai_8bd7bc = 0;
  g_weapon_ai_8be1f0 = 0;
  g_weapon_ai_8bdcc0 = 0;
  g_weapon_8be1e4 = 0;
  g_sound_8d9940 = 0;
  g_weapon_ai_8be1e8 = 0;
  g_weapon_8d9944 = 0;
  g_weapon_ai_8be1ec = 0;
  g_weapon_ai_8d9948 = 0;
  /* zero the g_weapon_8be720 table one dword at a time */
  cursor = &g_weapon_8be720;
  for (countdown = WEAPON_8BE720_WORD_COUNT; countdown != 0; countdown = countdown + -1) {
    *cursor = 0;
    cursor = cursor + 1;
  }
  g_weapon_ai_8d994c = 0;
  return;
}


/* array_find_index */

/* "(value, array, count): index of value or -1" */

extern int array_find_index(char *param_1,int *param_2,char *param_3);


/* sound_find_active_voice_for_slot */

/* "searches the 4 voice records for an active one with matching id and slot" */

extern undefined4 __cdecl sound_find_active_voice_for_slot(char *param_1,char *param_2);
extern char g_sound_8be7c4[];

/* bytes per record of g_weapon_610dfc (inferred) */
#define WEAPON_CHAIN_RECORD_SIZE (0x28)
/* chain value that repeats the previous level (see weapon_stack_collect_types) */
#define WEAPON_CHAIN_REPEAT (0x25)
/* end of the g_sound_stream_slots scan: four pointers from 0x8BE7C0 (inferred) */
#define SOUND_STREAM_SLOTS_SCAN_END (((LONG_PTR)&g_sound_8be7c4 + 0xb))


/* weapon_stack_collect_types */

/* "collects the weapon slot types (0x25 = repeat marker) into stack arrays" */
/* Walks the weapon chain that starts at chain_index in g_weapon_610dfc: each record's first dword is
   the next chain value and a zero ends the chain. The values go into chain_types (one per level),
   with stack_ids / stack_params holding the matching per-level values. seed_record supplies the
   level-0 values (+4 and +8). Then the four g_sound_stream_slots are checked: returns 1 when an
   active stream (type 1 at +0xC) matches the collected levels at every depth, otherwise 0. Returns 0
   at once for an empty chain. */

/* TIER2: unvalidated 2026-10-17 authority=diffemu */
undefined4 __cdecl weapon_stack_collect_types(int chain_index,char *seed_record)
{
  char *stream;
  int stack_pos;
  int stack_depth;
  int *stream_slot;
  int stack_params [19];
  int stack_ids [21];
  int chain_types [20];

  if (*(int *)((char *)&g_weapon_610dfc + chain_index * WEAPON_CHAIN_RECORD_SIZE) == 0) {
    return 0;
  }
  stack_depth = 0;
  /* one level per chain value: a repeat copies the level below, level 0 is seeded from seed_record,
     other levels shift the level below (inferred). A repeat as the very first value reads index -1. */
  do {
    chain_index = *(int *)((char *)&g_weapon_610dfc + chain_index * WEAPON_CHAIN_RECORD_SIZE);
    chain_types[stack_depth] = chain_index;
    if (chain_index == WEAPON_CHAIN_REPEAT) {
      stack_ids[stack_depth + 1] = stack_ids[stack_depth];
      stack_params[stack_depth] = stack_params[stack_depth + -1];
    }
    else if (stack_depth < 1) {
      stack_ids[1] = *(undefined4 *)(seed_record + 8);
      stack_params[0] = *(int *)(seed_record + 4);
    }
    else {
      stack_ids[stack_depth + 1] = stack_params[stack_depth + -1];
      stack_params[stack_depth] = stack_ids[stack_depth];
    }
    stack_depth = stack_depth + 1;
  } while (*(int *)((char *)&g_weapon_610dfc + chain_index * WEAPON_CHAIN_RECORD_SIZE) != 0);
  /* scan the stream slots: a slot matches when its three fields equal the collected levels at every depth */
  stream_slot = &g_sound_stream_slots;
  do {
    stream = *stream_slot;
    if (((stream != 0) && (*(int *)(stream + 0xc) == 1)) && (stack_pos = 0, 0 < stack_depth)) {
      do {
        if (((*(int *)(stream + 0x28) == chain_types[stack_pos]) &&
            (*(int *)(stream + 4) == stack_ids[stack_pos + 1])) &&
           (*(int *)(stream + 0x14) == stack_params[stack_pos])) {
          return 1;
        }
        stack_pos = stack_pos + 1;
      } while (stack_pos < stack_depth);
    }
    stream_slot = stream_slot + 1;
    if (SOUND_STREAM_SLOTS_SCAN_END < (LONG_PTR)stream_slot) {
      return 0;
    }
  } while( true );
}


/* voice_queue_assign_slot */

/* "(player, line): points g_sound_8be7c0[player] at the record of the line, table chosen by the
   player's team" */

extern void voice_queue_assign_slot(char *param_1,char *param_2);


/* sound_phase_change_check */

/* "calls race_get_race_652dec when the race phase differs from the stored one" */

extern void sound_phase_change_check(void);


/* sound_tick_counter_increment */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* "g_weapon_6aaf74 += 1" */

extern void sound_tick_counter_increment(void);


/* sound_ducking_values_decay */

/* "steps each float of g_weapon_8be720 (6 per row) toward 0 through -1 and -2" */

extern void sound_ducking_values_decay(void);


/* voice_queue_clear_players_when_phase_2 */

/* "removes the voice lines of the four players from both queues when the race phase is 2" */

extern void voice_queue_clear_players_when_phase_2(void);

/* frames a car must stay without a voice line before the counter re-arms it (inferred) */
#define VOICE_IDLE_FRAMES (1000)


/* player_voice_counter_increment */

/* "increments g_car_6aaf64[player] and returns whether voices are enabled (option slider 4)" */
/* Per-frame voice gate for one player (player = car / slot index, also the g_player_slots index).
   Bumps the player's idle frame counter, then returns 1 when this player may start a voice line
   now and 0 otherwise. The counter is reset to 0 when a line is allowed by the idle-time path.
   Locals: value is reused as the slider value, the slot type and then the loop index;
   mode_or_result holds game_mode and then the results of the calls below. */

/* TIER2: validated 2026-10-22 authority=diffemu */
undefined4 __cdecl player_voice_counter_increment(int player)
{
  int *voice_counter;
  int value;
  int mode_or_result;

  mode_or_result = game_mode;
  voice_counter = &g_car_6aaf64 + player;
  *voice_counter = (&g_car_6aaf64)[player] + 1;
  value = option_get_slider_4();
  if (value != 0) {
    value = race_setup_get_slot_type(player);
    if ((value < 2) && (value = option_get_slider_4(), value == 2)) {
      if (g_player_inputs[g_player_slots[player].controller].insult == 0) {
        return 0;
      }
      if (g_player_inputs_previous[g_player_slots[player].controller].insult == 0) {
        return 1;
      }
      return 0;
    }
    /* block the line while any other player has an active voice stream (type 1) */
    value = 0;
    if (0 < g_sound_player_count) {
      do {
        if (((value != player) && ((&g_sound_stream_slots)[value] != 0)) &&
           (*(int *)((&g_sound_stream_slots)[value] + 0xc) == 1)) {
          return 0;
        }
        value = value + 1;
      } while (value < g_sound_player_count);
    }
    if ((mode_or_result == 4) && (mode_or_result = player_voice_progress_gap_check(player), mode_or_result == 0)) {
      return 0;
    }
    /* only for players whose flag in g_weapon_8be200 is 1: a loud enough stream allows the line at once,
       otherwise the idle counter must exceed VOICE_IDLE_FRAMES and the car must be playable */
    if (((int *)&g_weapon_8be200)[player] == 1) {
      if (k_f_63 <*(float *)(*(char **)((char *)&g_sound_8be7e0 + player * 4) + 0x14)) {
        return 1;
      }
      if ((VOICE_IDLE_FRAMES < *voice_counter) &&(mode_or_result = car_is_playable(player), mode_or_result != 0)) {
        *voice_counter = 0;
        return 1;
      }
    }
  }
  return 0;
}


/* voice_queue_player_update */

/* "per player: counts frames (player_voice_counter_increment), updates its queue entries and
   voices" */

extern void voice_queue_player_update(char *param_1);


/* voice_queue_final_lines */

/* "queues the end of race lines (winner / last active player) depending on the race phase and game
   mode" */

extern void voice_queue_final_lines(void);


/* voice_queue_frame_update */

/* "per frame entry: tick counter, race queue update (not for event types 0 and 1), player updates
   and pending lines" */

extern void voice_queue_frame_update(void);


/* sound_listener_update */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* "places the sound listener from the viewport camera matrix and position (validated with
   vec3_is_valid_small)" */

extern void sound_listener_update(void);


/* sound_music_fade_step */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* "(dir): g_sound_6ad8ac -= 0.2 / += 0.2 / -= 0.01, clamped to 1.0" */

extern void sound_music_fade_step(char *param_1);


/* sound_get_8d9960 */

/* "g_sound_8d9960[index]" */

extern undefined4 sound_get_8d9960(char *param_1);


/* player_voice_progress_gap_check */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* "true when the progress gap between two players is in (-15, 10)" */

extern undefined4 __cdecl player_voice_progress_gap_check(char *param_1);


/* sound_spline_create_a */

/* "RpSplineCreate: ambience path of a course" */

extern void sound_spline_create_a(void);


/* sound_spline_create_b */

/* "RpSplineCreate: ambience path of a course" */

extern void sound_spline_create_b(void);


/* sound_flag_617af0_set */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* "_g_sound_617af0 = 1" */

extern void sound_flag_617af0_set(void);


/* sound_flag_617af0_clear */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* "_g_sound_617af0 = 0" */

extern void sound_flag_617af0_clear(void);


/* sound_music_for_career_row */

/* "switch on the career row returning the music id" */

extern undefined4 sound_music_for_career_row(void);


/* sound_stub_return_0 */

/* WARNING: Switch with 1 destination removed at 0x0046ef42 : 7 cases all go to same destination */
/* "returns without work" */

/* Sound stub: takes no parameters, does nothing, returns 0.
 * Used as a no-op stand-in for a sound function (inferred: 7 switch cases all route here). */

/* TIER2: validated 2026-10-11 authority=diffemu */
undefined4 sound_stub_return_0(void)
{
  return 0;
}


/* sound_update */

/* "sound_fn_46a010 (race sounds) + sound_fn_473450 (voice update)" */

extern void sound_update(void);


/* course_unload_atomics_6ae24c */

/* "destroys the atomics of the 0x68 byte records from 0x6AE24C" */

extern void course_unload_atomics_6ae24c(void);


/* sound_get_6af57c */

/* "returns &g_sound_6af57c" */

extern undefined * sound_get_6af57c(void);


