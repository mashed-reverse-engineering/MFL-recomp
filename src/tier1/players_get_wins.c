#include "mfl_tier2.h"
/*0041a380*/ extern int ai_ftol_noop_wrapper();
/*007ef7b0*/ extern undefined4 g_car_7ef7b0;
/*007ef7b8*/ extern undefined4 game_mode;
/*007f01fc*/ extern struct PlayerSlot g_player_slots[4];
/*008d8b50*/ extern undefined4 g_player_8d8b50;
extern char g_ai_editor_mode[];
#define PLAYER_SLOT_COUNT (4)
#define PLAYER_SLOTS_END (((LONG_PTR)&g_ai_editor_mode)) /* one past the end of g_player_slots */
#define WINS_ABSENT_PLAYER (-1) /* wins sort key of a slot with no player (sorted last) */
/* round_scores_reset */
/* "sets the three per player change tables at 0x8D8B60 to -1000 (0xFFFFFC18)" */
/* player_get_points_change */
/* "0x8D8B80[player]" */
/* player_get_points_change_visual */
/* "0x8D8B60[player]" */
/* players_get_wins */
/* Ranks the 4 player slots by their wins (g_player_8d8b50 table). out_order[k] receives the index of
   the player in rank k, best first (out_order is an undefined4[4] output). Absent players
   (controller < 0) use WINS_ABSENT_PLAYER and sort last. Equal wins keep the lower player index first.
   Game modes 4 / 7 overwrite out_order with ai_ftol_noop_wrapper(i) results, and game mode 9 swaps
   out_order[0] / out_order[1] depending on g_car_7ef7b0 (both inferred). Returns nothing. */
/* "(out[4]): player indices sorted by wins, -1 for absent players (inferred)" */
void __cdecl players_get_wins(undefined4 *out_order)
{
  int cur_stat;
  int next_stat;
  int i;
  undefined4 out_value;
  PlayerSlot *slot;
  int pos;
  int wins [PLAYER_SLOT_COUNT];

  /* fill wins[] from 0x8D8B50 for each slot (or WINS_ABSENT_PLAYER) and seed out_order with 0..3;
     i is the player index here and the pass count in the sort below (inferred: reused variable) */
  i = 0;
  slot = g_player_slots;
  do {
    wins[i] = (&g_player_8d8b50)[i];
    if (slot->controller < 0) {
      wins[i] = WINS_ABSENT_PLAYER;
    }
    out_order[i] = i;
    i = i + 1;
    slot = slot + 1;
  } while ((LONG_PTR)slot < PLAYER_SLOTS_END);
  /* bubble sort, descending by wins: PLAYER_SLOT_COUNT - 1 passes, each swap also moves the
     matching out_order entries */
  i = PLAYER_SLOT_COUNT - 1;
  do {
    pos = 0;
    if (0 < i) {
      do {
        next_stat = wins[pos + 1];
        if (wins[pos] < next_stat) {
          cur_stat = wins[pos];
          wins[pos] = next_stat;
          wins[pos + 1] = cur_stat;
          out_value = out_order[pos];
          out_order[pos] = out_order[pos + 1];
          out_order[pos + 1] = out_value;
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
      out_value = ai_ftol_noop_wrapper(i);
      out_order[i] = out_value;
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
