#include "mfl_tier2.h"
/*00409eb0*/ extern float player_rank_key();
/*0040cc60*/ extern int player_get_points();
/*0040fdd0*/ extern int race_set_phase();
/*0040ff10*/ extern int race_set_view_player();
/*00421e20*/ extern int vehicle_set_active();
/*004286f0*/ extern int player_stats_debug_log();
/*0042e980*/ extern int course_get_course_8c879c();
/*0042ecb0*/ extern int intro_banner_start_by_event(void);
/*0042fcb0*/ extern int menu_reset_scroll();
/*00434a10*/ extern int is_team_mode();
/*00434bb0*/ extern int get_event_type();
/*00442550*/ extern int career_mark_row_unlocked();
/*00451c50*/ extern int camera_records_warmup();
/*0047a830*/ extern int car_is_active();
/*004b2b48*/ extern int _ftol();
/*004b2bbd*/ extern int __cdecl FID_conflict__wprintf(char *p0, ...);
/*006037c0*/ extern undefined4 g_phase_timer;
/*00652e04*/ extern undefined4 g_race_course_loaded_event;
/*00652f5c*/ extern undefined4 g_race_652f5c;
/*007ef7dc*/ extern undefined4 g_race_clock_a;
/*007ef7fc*/ extern undefined4 g_race_7ef7fc;
/*007ef800*/ extern undefined4 g_time_limit_ticks;
/*007ef820*/ extern struct PlayerInput g_player_inputs[12];
/*007efce0*/ extern struct PlayerInput g_player_inputs_previous[12];
/*007f01fc*/ extern struct PlayerSlot g_player_slots[4];
/*008d8b14*/ extern char *g_race_setup;
/*008d8b30*/ extern undefined4 player_count;

/* Offset of the first per-player int in g_race_setup; 4 bytes per player (inferred). */
#define RACE_SETUP_PLAYER_BASE (0xb8)
/* End offset (exclusive) of the per-player ints in g_race_setup (inferred). */
#define RACE_SETUP_PLAYER_END (200)
/* Number of player slots, the size of g_player_slots. */
#define PLAYER_SLOT_COUNT (4)
/* Phase timer value used when no car is active (inferred). */
#define PHASE_TIMER_RESET (400)
/* Phase timer must reach this before a start press loads the race (inferred). */
#define PHASE_TIMER_READY (200)
/* Points score that wins a 2-3 player or team game (inferred). */
#define WIN_POINTS_SMALL_GAME (8)
/* In 4-player games a score strictly above this wins (inferred). */
#define POINTS_4P_THRESHOLD (11)
/* Course id 11 (enum CourseId). */
#define COURSE_SANDS (11)
/* Event type that always takes the phase 8 path (inferred). */

/* race_phase_7_update */

/* "phase 7 handler of game_tick (race over: clocks stopped)" */

/*
 * Phase 7 handler of the race state machine, called once per game tick (inferred).
 * Takes no parameters and returns nothing.
 * Steps: note whether the time limit has passed, find the active cars, advance the
 * phase timer, wait for a menu_select press once the timer is ready, then move to
 * phase 9 (winner view, camera warmup) or phase 8 (race end).
 */
void race_phase_7_update(void)

{
  bool start_pressed;  /* set when a player presses menu_select this tick */
  int check_result;    /* return value of car_is_active / is_team_mode */
  int winner_marker;   /* reused: last active car index (loop 1), active car count (loop 2), 1 + winner slot later (inferred) */
  int scratch;         /* reused: per-player offset, phase timer, event type, player loop index */
  PlayerSlot *slot_ptr;
  int points;          /* car slot index in loops 1-2, player points afterwards */

  /* Time limit flag: set once the race clock is past the time limit (inferred). */
  g_race_7ef7fc = (uint)((int)g_time_limit_ticks < (int)g_race_clock_a);
  intro_banner_start_by_event();
  /* Loop 1: remember the last car slot that is present and active; its rank key goes to g_race_652f5c. */
  winner_marker = 0;
  points = 0;
  scratch = RACE_SETUP_PLAYER_BASE;
  do {
    if ((*(int *)(scratch + g_race_setup) != 0) && (check_result = car_is_active(points), check_result == 1)) {
      winner_marker = points;
    }
    scratch = scratch + 4;
    points = points + 1;
  } while (scratch < RACE_SETUP_PLAYER_END);
  g_race_652f5c = (int)player_rank_key(winner_marker);
  /* Loop 2: count the present, active cars into winner_marker. */
  winner_marker = 0;
  points = 0;
  scratch = RACE_SETUP_PLAYER_BASE;
  do {
    if ((*(int *)(scratch + g_race_setup) != 0) && (check_result = car_is_active(points), check_result == 1)) {
      winner_marker = winner_marker + 1;
    }
    scratch = scratch + 4;
    points = points + 1;
  } while (scratch < RACE_SETUP_PLAYER_END);
  /* Phase timer: restart from PHASE_TIMER_RESET when no car is active, then advance by 2 per tick. */
  scratch = PHASE_TIMER_RESET;
  if (winner_marker != 0) {
    scratch = g_phase_timer;
  }
  g_phase_timer = scratch + 2;
  /* Start press: per player, menu_select went from 0 to non-zero this tick (rising edge).
     The race is loaded (g_race_course_loaded_event = 1) only when the timer is ready;
     the literal 199 with "<" means timer >= PHASE_TIMER_READY. Players 1-4 are written out one by one. */
  start_pressed = false;
  if ((((g_player_slots[0].controller != -1) &&
       (g_player_inputs[g_player_slots[0].controller].menu_select != 0)) &&
      (g_player_inputs_previous[g_player_slots[0].controller].menu_select == 0)) &&
     (start_pressed = true, 199 < (int)g_phase_timer)) {
    g_race_course_loaded_event = 1;
  }
  if (((g_player_slots[1].controller != -1) &&
      (g_player_inputs[g_player_slots[1].controller].menu_select != 0)) &&
     ((g_player_inputs_previous[g_player_slots[1].controller].menu_select == 0 &&
      (start_pressed = true, 199 < (int)g_phase_timer)))) {
    g_race_course_loaded_event = 1;
  }
  if (((g_player_slots[2].controller != -1) &&
      (g_player_inputs[g_player_slots[2].controller].menu_select != 0)) &&
     ((g_player_inputs_previous[g_player_slots[2].controller].menu_select == 0 &&
      (start_pressed = true, 199 < (int)g_phase_timer)))) {
    g_race_course_loaded_event = 1;
  }
  if (((g_player_slots[3].controller != -1) &&
      (g_player_inputs[g_player_slots[3].controller].menu_select != 0)) &&
     (g_player_inputs_previous[g_player_slots[3].controller].menu_select == 0)) {
    start_pressed = true;
    if ((int)g_phase_timer < PHASE_TIMER_READY) {
      return;
    }
    g_race_course_loaded_event = 1;
  }
  /* Timer not ready yet, or nobody pressed start this tick: nothing to do. */
  if ((int)g_phase_timer < PHASE_TIMER_READY) {
    return;
  }
  if (!start_pressed) {
    return;
  }
  menu_reset_scroll();
  /* Loop 3: print each player's points; winner_marker = slot + 1 for a winning score
     (2-3 players or team mode: points == WIN_POINTS_SMALL_GAME; 4 players: points > POINTS_4P_THRESHOLD). */
  winner_marker = 0;
  scratch = 0;
  do {
    points = player_get_points(scratch);
    FID_conflict__wprintf("player %d points : %d\n",scratch,points);
    if ((((player_count == 2) || (player_count == 3)) || (check_result = is_team_mode(), check_result != 0)) &&
       (points == WIN_POINTS_SMALL_GAME)) {
      winner_marker = scratch + 1;
    }
    if ((player_count == 4) && (POINTS_4P_THRESHOLD < points)) {
      winner_marker = scratch + 1;
    }
    scratch = scratch + 1;
  } while (scratch < PLAYER_SLOT_COUNT);
  /* Event types 0, 1 or 2 on COURSE_SANDS: mark the career row unlocked and set winner_marker = 1 (inferred). */
  scratch = get_event_type();
  if ((((scratch == EVENT_TIME_TRIAL_0) || (scratch = get_event_type(), scratch == 1)) ||
      (scratch = get_event_type(), scratch == 2)) &&
     (scratch = course_get_course_8c879c(), scratch == COURSE_SANDS)) {
    winner_marker = 1;
    career_mark_row_unlocked();
  }
  /* Winner found and event not EVENT_TRAINING: camera warmup for the winner, race_set_view_player(1), go to phase 9. */
  scratch = get_event_type();
  if ((scratch != EVENT_TRAINING) && (winner_marker != 0)) {
    camera_records_warmup(1,winner_marker + -1);
    race_set_view_player(1);
    race_set_phase(RACE_PHASE_WINNER_VIEW);
    return;
  }
  /* Otherwise: phase 8, camera_records_warmup(0,0), player_stats_debug_log(). */
  race_set_phase(RACE_PHASE_FINISHED);
  camera_records_warmup(0,0);
  player_stats_debug_log();
  /* Loop 4: activate the vehicle of every occupied player slot (controller != -1). */
  winner_marker = 0;
  slot_ptr = g_player_slots;
  do {
    if (slot_ptr->controller != -1) {
      vehicle_set_active(winner_marker,1);
    }
    slot_ptr = slot_ptr + 1;
    winner_marker = winner_marker + 1;
  } while ((char *)slot_ptr < (char *)&g_player_slots[PLAYER_SLOT_COUNT]);
  return;
}


