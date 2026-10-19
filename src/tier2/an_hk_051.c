#include "mfl_tier2.h"
/*007e8b64*/ extern EventLogArgs g_event_log_args[256];
typedef int CourseId;
typedef int WeaponId;
/*0040fdb0*/ extern int get_player_count();
/*0040fdc0*/ extern int race_get_phase();
/*0040fe20*/ extern int __cdecl race_setup_slot_used(undefined4 p0);
/*00434a10*/ extern int is_team_mode();
/*007e7f60*/ extern undefined4 g_event_log_head;
/*007e7f64*/ extern undefined4 g_event_log_type;
/*007e8364*/ extern undefined4 g_event_log_time;
/*007e8764*/ extern undefined4 g_event_log_param;
/*007ef7b8*/ extern undefined4 game_mode;
/*007ef7bc*/ extern undefined4 g_cars_7ef7bc;
/*007ef7fc*/ extern undefined4 g_race_7ef7fc;
/*007ef818*/ extern undefined4 g_last_frame_ticks;
/*007f01fc*/ extern struct PlayerSlot g_player_slots[4];
/*008d8b40*/ extern undefined4 player_points;
/*008d8b60*/ extern char *g_round_8d8b60;
/*008d8b70*/ extern char *g_round_8d8b70;
/*008d8b80*/ extern char *g_round_8d8b80;
/*008d8bd0*/ extern char *g_round_8d8bd0;

#define MAX_PLAYERS (4)
#define EVENT_LOG_MAX_INDEX (0xff)
#define EVENT_TYPE_PLAYER_POINTS (0x2ff01)


/* player_add_points */

/* "(see util names)" */

/* Adds points to the total of one player and logs the award.
 * player: index into g_player_slots / player_points (inferred: 0..3).
 * points: amount to add; may be negative (inferred: 2 is a special value, see the mode checks).
 * The amount is filtered by game mode first, then stored in the per-round globals, then added to
 * player_points[player] with the total clamped to 0. Returns nothing. */
void player_add_points(int player,uint points)

{
  int *total_ptr;
  undefined4 frame_ticks;
  int slot_used;
  int result;
  int slot;
  bool flag;

  if (game_mode == 1) {
    if ((int)points < 0) {
      points = 0;
    }
    /* Race mode: count the player slots in use. */
    result = 0;
    slot = 0;
    do {
      slot_used = race_setup_slot_used(slot);
      if (slot_used != 0) {
        result = result + 1;
      }
      slot = slot + 1;
    } while (slot < MAX_PLAYERS);
    /* All slots used and not team mode: the amount becomes 1 if it is 2, else 0. */
    if (result == MAX_PLAYERS) {
      result = is_team_mode();
      if (result == 0) {
        points = (uint)(points == 2);
      }
    }
    /* Only the current car (solo) or a car with the same slot state (team) scores. */
    result = is_team_mode();
    if (result == 0) {
      flag = player == g_cars_7ef7bc;
    }
    else {
      flag = g_player_slots[g_cars_7ef7bc].state == g_player_slots[player].state;
    }
    if (!flag) {
      points = 0;
    }
  }
  /* Mode 2: no points for slot state 1; with all slots used the amount becomes 1 if it is 2, else 0; otherwise 2 becomes 1. */
  if (game_mode == 2) {
    if ((int)points < 0) {
      points = 0;
    }
    if (g_player_slots[player].state == 1) {
      points = 0;
    }
    result = get_player_count();
    if (result == MAX_PLAYERS) {
      points = (uint)(points == 2);
    }
    else if (points == 2) {
      points = 1;
    }
  }
  *(undefined4 *)((char *)&g_round_8d8bd0 + player * 4) = (&player_points)[player];
  flag = g_race_7ef7fc != 0;
  *(uint *)((char *)&g_round_8d8b80 + player * 4) = points;
  if ((flag) && ((int)points < 0)) {
    points = 0;
  }
  *(uint *)((char *)&g_round_8d8b60 + player * 4) = points;
  /* Log the award to the event ring buffer unless the race phase is 1..5 (inferred: not while racing or in results). */
  result = race_get_phase();
  frame_ticks = g_last_frame_ticks;
  if ((((result != 1) && (result != 2)) && (result != 3)) && ((result != 4 && (result != 5)))) {
    (&g_event_log_type)[g_event_log_head] = EVENT_TYPE_PLAYER_POINTS;
    (&g_event_log_time)[g_event_log_head] = frame_ticks;
    (&g_event_log_param)[g_event_log_head] = player;
    g_event_log_args[g_event_log_head].arg0 = points;
    g_event_log_head = g_event_log_head + 1;
    if (EVENT_LOG_MAX_INDEX < g_event_log_head) {
      g_event_log_head = 0;
    }
  }
  /* Add to the running total; a total below 0 is clamped to 0. */
  total_ptr = &player_points + player;
  *total_ptr = *total_ptr + points;
  result = *total_ptr;
  *(undefined4 *)((char *)&g_round_8d8b70 + player * 4) = 6000;
  if (result < 0) {
    (&player_points)[player] = 0;
  }
  return;
}
