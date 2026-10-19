#include "mfl_tier2.h"
/*0040fdb0*/ extern int get_player_count();
/*004276b0*/ extern int __cdecl player_stat_get_crowns(undefined4 p0);
/*004276d0*/ extern int __cdecl player_stat_get_pants(undefined4 p0);
/*004276f0*/ extern int __cdecl player_stat_get_crowns_total(undefined4 p0);
/*00427710*/ extern int __cdecl player_stat_get_pants_total(undefined4 p0);
/*00427730*/ extern int player_stat_get_turbo_used_team();
/*004277b0*/ extern int player_stat_get_aggression_team();
/*00427830*/ extern int __cdecl player_stat_get_kills(undefined4 p0);
/*004278c0*/ extern int player_stat_get_wall_hits_team();
/*00427940*/ extern int __cdecl player_stat_get_unforced_errors(undefined4 p0);
/*004279d0*/ extern int player_stat_get_turbo_used_total_team();
/*00427a50*/ extern int player_stat_get_aggression_total_team();
/*00427ad0*/ extern int __cdecl player_stat_get_kills_total(undefined4 p0);
/*00427b60*/ extern int player_stat_get_wall_hits_total_team();
/*00427be0*/ extern int __cdecl player_stat_get_unforced_errors_total(undefined4 p0);
/*00434a10*/ extern int is_team_mode();
/*007f01fc*/ extern struct PlayerSlot g_player_slots[4];
/*008d8b30*/ extern undefined4 player_count;
/*007f0240*/ extern char g_ai_editor_view_origin_x[];
/*007f023c*/ extern char g_ai_editor_mode[];

/* stat kinds (stat_kind): each names the player_stat_get_* callee its case calls */
#define STAT_CROWNS (0)
#define STAT_PANTS (1)
#define STAT_TURBO_USED_TEAM (2)
#define STAT_AGGRESSION_TEAM (3)
#define STAT_KILLS (4)
#define STAT_WALL_HITS_TEAM (5)
#define STAT_UNFORCED_ERRORS (6)
#define STAT_CROWNS_TOTAL (7)
#define STAT_PANTS_TOTAL (8)
#define STAT_TURBO_USED_TOTAL_TEAM (9)
#define STAT_AGGRESSION_TOTAL_TEAM (10)
#define STAT_KILLS_TOTAL (11)
#define STAT_WALL_HITS_TOTAL_TEAM (12)
#define STAT_UNFORCED_ERRORS_TOTAL (13)

/* stat_values entry for a slot with no controller (wall hits use a large value instead) */
#define NO_PLAYER_STAT_VALUE (-100)
#define NO_PLAYER_WALL_HITS_VALUE (100000)

/* player_stat_get_by_kind */

/* Fills player_order[0..3] with the player slot indices (0..3), sorted by one statistic.
 * stat_kind: a STAT_* value; selects the player_stat_get_* callee for the values.
 * ascending: 0 = largest value first, nonzero = smallest value first.
 * Team mode (is_team_mode(), and not STAT_CROWNS / STAT_PANTS / their totals): each slot's value is replaced by
 * its team's sum (slot state 0 or 1, inferred: the two teams), then the top entries are adjusted so that
 * player_order[0..2] do not all share a team (inferred).
 * (was: "(player, kind): switch over the statistic kinds")
 */

void __cdecl player_stat_get_by_kind(int *player_order,int stat_kind,int ascending)

{
  int swap_offset; /* byte offset of the pair being swapped in player_order */
  undefined4 tmp_next_player;
  undefined4 tmp_player;
  int *slot_state_ptr;
  int current_value; /* slot state (team sums), then the value being sorted */
  int *stat_ptr;
  PlayerSlot *slot_ptr;
  int out_minus_stats; /* byte distance from stat_values to player_order; also is_team_mode() at the end */
  int player_idx; /* loop index over slots and sort passes; also the is_team_mode() flag and team 0 sum */
  int stat_value; /* per-slot stat; also the team 1 sum and get_player_count() */
  int stat_values [4];

  /* one loop per stat kind: stat_values[i] = callee(i) for slots with a controller, else the no-player value;
     player_order[i] = i. Loops end at &g_ai_editor_mode, the end of g_player_slots */
  switch(stat_kind) {
  case STAT_CROWNS:
    player_idx = 0;
    slot_ptr = g_player_slots;
    out_minus_stats = (int)player_order - (int)stat_values;
    do {
      if (slot_ptr->controller == -1) {
        stat_values[player_idx] = NO_PLAYER_STAT_VALUE;
      }
      else {
        stat_value = player_stat_get_crowns(player_idx);
        stat_values[player_idx] = stat_value;
      }
      *(int *)((int)stat_values + out_minus_stats + player_idx * 4) = player_idx;
      slot_ptr = slot_ptr + 1;
      player_idx = player_idx + 1;
    } while ((int)slot_ptr < ((int)&g_ai_editor_mode));
    break;
  case STAT_PANTS:
    player_idx = 0;
    slot_ptr = g_player_slots;
    out_minus_stats = (int)player_order - (int)stat_values;
    do {
      if (slot_ptr->controller == -1) {
        stat_values[player_idx] = NO_PLAYER_STAT_VALUE;
      }
      else {
        stat_value = player_stat_get_pants(player_idx);
        stat_values[player_idx] = stat_value;
      }
      *(int *)((int)stat_values + out_minus_stats + player_idx * 4) = player_idx;
      slot_ptr = slot_ptr + 1;
      player_idx = player_idx + 1;
    } while ((int)slot_ptr < ((int)&g_ai_editor_mode));
    break;
  case STAT_TURBO_USED_TEAM:
    player_idx = 0;
    slot_ptr = g_player_slots;
    out_minus_stats = (int)player_order - (int)stat_values;
    do {
      if (slot_ptr->controller == -1) {
        stat_values[player_idx] = NO_PLAYER_STAT_VALUE;
      }
      else {
        stat_value = player_stat_get_turbo_used_team(player_idx);
        stat_values[player_idx] = stat_value;
      }
      *(int *)((int)stat_values + out_minus_stats + player_idx * 4) = player_idx;
      slot_ptr = slot_ptr + 1;
      player_idx = player_idx + 1;
    } while ((int)slot_ptr < ((int)&g_ai_editor_mode));
    break;
  case STAT_AGGRESSION_TEAM:
    player_idx = 0;
    slot_ptr = g_player_slots;
    out_minus_stats = (int)player_order - (int)stat_values;
    do {
      if (slot_ptr->controller == -1) {
        stat_values[player_idx] = NO_PLAYER_STAT_VALUE;
      }
      else {
        stat_value = player_stat_get_aggression_team(player_idx);
        stat_values[player_idx] = stat_value;
      }
      *(int *)((int)stat_values + out_minus_stats + player_idx * 4) = player_idx;
      slot_ptr = slot_ptr + 1;
      player_idx = player_idx + 1;
    } while ((int)slot_ptr < ((int)&g_ai_editor_mode));
    break;
  case STAT_KILLS:
    player_idx = 0;
    slot_ptr = g_player_slots;
    out_minus_stats = (int)player_order - (int)stat_values;
    do {
      if (slot_ptr->controller == -1) {
        stat_values[player_idx] = NO_PLAYER_STAT_VALUE;
      }
      else {
        stat_value = player_stat_get_kills(player_idx);
        stat_values[player_idx] = stat_value;
      }
      *(int *)((int)stat_values + out_minus_stats + player_idx * 4) = player_idx;
      slot_ptr = slot_ptr + 1;
      player_idx = player_idx + 1;
    } while ((int)slot_ptr < ((int)&g_ai_editor_mode));
    break;
  case STAT_WALL_HITS_TEAM:
    player_idx = 0;
    slot_ptr = g_player_slots;
    out_minus_stats = (int)player_order - (int)stat_values;
    do {
      if (slot_ptr->controller == -1) {
        stat_values[player_idx] = NO_PLAYER_WALL_HITS_VALUE;
      }
      else {
        stat_value = player_stat_get_wall_hits_team(player_idx);
        stat_values[player_idx] = stat_value;
      }
      *(int *)((int)stat_values + out_minus_stats + player_idx * 4) = player_idx;
      slot_ptr = slot_ptr + 1;
      player_idx = player_idx + 1;
    } while ((int)slot_ptr < ((int)&g_ai_editor_mode));
    break;
  case STAT_UNFORCED_ERRORS:
    player_idx = 0;
    slot_ptr = g_player_slots;
    out_minus_stats = (int)player_order - (int)stat_values;
    do {
      if (slot_ptr->controller == -1) {
        stat_values[player_idx] = NO_PLAYER_STAT_VALUE;
      }
      else {
        stat_value = player_stat_get_unforced_errors(player_idx);
        stat_values[player_idx] = stat_value;
      }
      *(int *)((int)stat_values + out_minus_stats + player_idx * 4) = player_idx;
      slot_ptr = slot_ptr + 1;
      player_idx = player_idx + 1;
    } while ((int)slot_ptr < ((int)&g_ai_editor_mode));
    break;
  case STAT_CROWNS_TOTAL:
    player_idx = 0;
    slot_ptr = g_player_slots;
    out_minus_stats = (int)player_order - (int)stat_values;
    do {
      if (slot_ptr->controller == -1) {
        stat_values[player_idx] = NO_PLAYER_STAT_VALUE;
      }
      else {
        stat_value = player_stat_get_crowns_total(player_idx);
        stat_values[player_idx] = stat_value;
      }
      *(int *)((int)stat_values + out_minus_stats + player_idx * 4) = player_idx;
      slot_ptr = slot_ptr + 1;
      player_idx = player_idx + 1;
    } while ((int)slot_ptr < ((int)&g_ai_editor_mode));
    break;
  case STAT_PANTS_TOTAL:
    player_idx = 0;
    slot_ptr = g_player_slots;
    out_minus_stats = (int)player_order - (int)stat_values;
    do {
      if (slot_ptr->controller == -1) {
        stat_values[player_idx] = NO_PLAYER_STAT_VALUE;
      }
      else {
        stat_value = player_stat_get_pants_total(player_idx);
        stat_values[player_idx] = stat_value;
      }
      *(int *)((int)stat_values + out_minus_stats + player_idx * 4) = player_idx;
      slot_ptr = slot_ptr + 1;
      player_idx = player_idx + 1;
    } while ((int)slot_ptr < ((int)&g_ai_editor_mode));
    break;
  case STAT_TURBO_USED_TOTAL_TEAM:
    player_idx = 0;
    slot_ptr = g_player_slots;
    out_minus_stats = (int)player_order - (int)stat_values;
    do {
      if (slot_ptr->controller == -1) {
        stat_values[player_idx] = NO_PLAYER_STAT_VALUE;
      }
      else {
        stat_value = player_stat_get_turbo_used_total_team(player_idx);
        stat_values[player_idx] = stat_value;
      }
      *(int *)((int)stat_values + out_minus_stats + player_idx * 4) = player_idx;
      slot_ptr = slot_ptr + 1;
      player_idx = player_idx + 1;
    } while ((int)slot_ptr < ((int)&g_ai_editor_mode));
    break;
  case STAT_AGGRESSION_TOTAL_TEAM:
    player_idx = 0;
    slot_ptr = g_player_slots;
    out_minus_stats = (int)player_order - (int)stat_values;
    do {
      if (slot_ptr->controller == -1) {
        stat_values[player_idx] = NO_PLAYER_STAT_VALUE;
      }
      else {
        stat_value = player_stat_get_aggression_total_team(player_idx);
        stat_values[player_idx] = stat_value;
      }
      *(int *)((int)stat_values + out_minus_stats + player_idx * 4) = player_idx;
      slot_ptr = slot_ptr + 1;
      player_idx = player_idx + 1;
    } while ((int)slot_ptr < ((int)&g_ai_editor_mode));
    break;
  case STAT_KILLS_TOTAL:
    player_idx = 0;
    slot_ptr = g_player_slots;
    out_minus_stats = (int)player_order - (int)stat_values;
    do {
      if (slot_ptr->controller == -1) {
        stat_values[player_idx] = NO_PLAYER_STAT_VALUE;
      }
      else {
        stat_value = player_stat_get_kills_total(player_idx);
        stat_values[player_idx] = stat_value;
      }
      *(int *)((int)stat_values + out_minus_stats + player_idx * 4) = player_idx;
      slot_ptr = slot_ptr + 1;
      player_idx = player_idx + 1;
    } while ((int)slot_ptr < ((int)&g_ai_editor_mode));
    break;
  case STAT_WALL_HITS_TOTAL_TEAM:
    player_idx = 0;
    slot_ptr = g_player_slots;
    out_minus_stats = (int)player_order - (int)stat_values;
    do {
      if (slot_ptr->controller == -1) {
        stat_values[player_idx] = NO_PLAYER_WALL_HITS_VALUE;
      }
      else {
        stat_value = player_stat_get_wall_hits_total_team(player_idx);
        stat_values[player_idx] = stat_value;
      }
      *(int *)((int)stat_values + out_minus_stats + player_idx * 4) = player_idx;
      slot_ptr = slot_ptr + 1;
      player_idx = player_idx + 1;
    } while ((int)slot_ptr < ((int)&g_ai_editor_mode));
    break;
  case STAT_UNFORCED_ERRORS_TOTAL:
    player_idx = 0;
    slot_ptr = g_player_slots;
    out_minus_stats = (int)player_order - (int)stat_values;
    do {
      if (slot_ptr->controller == -1) {
        stat_values[player_idx] = NO_PLAYER_STAT_VALUE;
      }
      else {
        stat_value = player_stat_get_unforced_errors_total(player_idx);
        stat_values[player_idx] = stat_value;
      }
      *(int *)((int)stat_values + out_minus_stats + player_idx * 4) = player_idx;
      slot_ptr = slot_ptr + 1;
      player_idx = player_idx + 1;
    } while ((int)slot_ptr < ((int)&g_ai_editor_mode));
    break;
  default:
    /* unknown kind: identity order */
    *player_order = 0;
    player_order[1] = 1;
    player_order[2] = 2;
    player_order[3] = 3;
    return;
  }
  player_idx = is_team_mode();
  /* team mode: sum the values of each team, then give every slot in that team the team sum */
  if ((((player_idx != 0) && (stat_kind != STAT_CROWNS)) && (stat_kind != STAT_PANTS)) && ((stat_kind != STAT_CROWNS_TOTAL && (stat_kind != STAT_PANTS_TOTAL)))
     ) {
    player_idx = 0;
    if (g_player_slots[0].state == 0) {
      player_idx = stat_values[0];
    }
    stat_value = 0;
    if (g_player_slots[0].state == 1) {
      stat_value = stat_values[0];
    }
    if (g_player_slots[1].state == 0) {
      player_idx = player_idx + stat_values[1];
    }
    if (g_player_slots[1].state == 1) {
      stat_value = stat_value + stat_values[1];
    }
    if (g_player_slots[2].state == 0) {
      player_idx = player_idx + stat_values[2];
    }
    if (g_player_slots[2].state == 1) {
      stat_value = stat_value + stat_values[2];
    }
    if (g_player_slots[3].state == 0) {
      player_idx = player_idx + stat_values[3];
    }
    if (g_player_slots[3].state == 1) {
      stat_value = stat_value + stat_values[3];
    }
    stat_ptr = stat_values;
    slot_state_ptr = &g_player_slots[0].state;
    do {
      current_value = *slot_state_ptr;
      if (current_value == 0) {
        *stat_ptr = player_idx;
      }
      if (current_value == 1) {
        *stat_ptr = stat_value;
      }
      slot_state_ptr = slot_state_ptr + 4;
      stat_ptr = stat_ptr + 1;
    } while ((int)slot_state_ptr < ((int)&g_ai_editor_view_origin_x));
  }
  /* bubble sort of player_order (through stat_values), one pass per player; passes use get_player_count() */
  player_idx = 0;
  if (ascending == 0) {
    /* ascending == 0: swap a pair when the left value is smaller, so the largest value ends first */
    stat_value = get_player_count();
    if (0 < stat_value) {
      do {
        stat_value = get_player_count();
        if (stat_value - player_idx != 1 && -1 < (stat_value - player_idx) + -1) {
          stat_value = 0;
          do {
            current_value = stat_values[stat_value];
            if (current_value < stat_values[stat_value + 1]) {
              stat_values[stat_value] = stat_values[stat_value + 1];
              swap_offset = out_minus_stats + stat_value * 4;
              tmp_next_player = *(undefined4 *)((int)stat_values + swap_offset + 4);
              stat_values[stat_value + 1] = current_value;
              tmp_player = *(undefined4 *)((int)stat_values + swap_offset);
              *(undefined4 *)((int)stat_values + swap_offset) = tmp_next_player;
              *(undefined4 *)((int)stat_values + swap_offset + 4) = tmp_player;
            }
            stat_value = stat_value + 1;
            current_value = get_player_count();
          } while (stat_value < (current_value - player_idx) + -1);
        }
        player_idx = player_idx + 1;
        stat_value = get_player_count();
      } while (player_idx < stat_value);
    }
  }
  else {
    stat_value = get_player_count();
    if (stat_value != 1 && -1 < stat_value + -1) {
      do {
        stat_value = get_player_count();
        if (stat_value - player_idx != 1 && -1 < (stat_value - player_idx) + -1) {
          stat_value = 0;
          do {
            current_value = stat_values[stat_value];
            if (stat_values[stat_value + 1] < current_value) {
              stat_values[stat_value] = stat_values[stat_value + 1];
              swap_offset = out_minus_stats + stat_value * 4;
              tmp_next_player = *(undefined4 *)((int)stat_values + swap_offset + 4);
              stat_values[stat_value + 1] = current_value;
              tmp_player = *(undefined4 *)((int)stat_values + swap_offset);
              *(undefined4 *)((int)stat_values + swap_offset) = tmp_next_player;
              *(undefined4 *)((int)stat_values + swap_offset + 4) = tmp_player;
            }
            stat_value = stat_value + 1;
            current_value = get_player_count();
          } while (stat_value < (current_value - player_idx) + -1);
        }
        player_idx = player_idx + 1;
        stat_value = get_player_count();
      } while (player_idx < stat_value + -1);
    }
  }
  /* team mode: keep player_order[0] and player_order[1] on different teams (slot state), and if the first three
     share a team, take player_order[3] (inferred) */
  out_minus_stats = is_team_mode();
  if (out_minus_stats != 0) {
    if (g_player_slots[*player_order].state == g_player_slots[player_order[1]].state) {
      player_order[1] = player_order[2];
    }
    if ((g_player_slots[*player_order].state == g_player_slots[player_order[1]].state) &&
       (g_player_slots[player_order[1]].state == g_player_slots[player_order[2]].state)) {
      player_order[1] = player_order[3];
      return;
    }
  }
  return;
}


