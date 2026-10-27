#include "mfl_tier2.h"
/*0040ff20*/ extern int race_setup_get_slot_type();
/*0044c180*/ extern double player_distance_get();
/*005dd330*/ extern float float_1_0;
/*005ddc28*/ extern float k_f_0;
/*005de20c*/ extern float k_f_2_5;
/*007ef7f0*/ extern undefined4 g_tick_count;
/*008c9ae0*/ extern struct AiBotRecord g_ai_bots[4];

#define RACE_SLOT_COUNT (4) /* slots scanned for the human player (inferred) */
#define RACE_SLOT_TYPE_HUMAN (1) /* race_setup_get_slot_type value of a human player (dossier) */
#define NO_LEADER_SLOT (-1)
#define LEADER_FAR_TIMEOUT_MS (72000) /* ms the leader must stay far away before returning 1 */

/* ai_find_leader_a */

/* "scans the players when player_distance_get(bot) is 0 (inferred)" */
/* Leader watch for bot_index (inferred from the body). Returns 1 once the human leader
   has stayed more than k_f_2_5 away for longer than LEADER_FAR_TIMEOUT_MS, else 0.
   Side effects: bot->leader_distance is set to the leader's distance, and bot->leader_far_ms
   accumulates g_tick_count while the leader is far, and is reset to 0 otherwise. */

undefined4 __cdecl ai_find_leader_a(int bot_index)

{
  struct AiBotRecord *bot;
  int slot_type;
  int slot_index;
  int leader_slot; /* NO_LEADER_SLOT, or the slot index of the human player; reused for the far-time total */
  double compare_value; /* player_distance_get(bot_index), then the k_f_2_5 threshold */
  double leader_dist;

  bot = &g_ai_bots[bot_index];
  leader_slot = NO_LEADER_SLOT;
  compare_value = (double)player_distance_get(bot_index);
  if (compare_value == (double)k_f_0) {
    /* scan all race slots; keep the last human slot index found */
    slot_index = 0;
    do {
      slot_type = race_setup_get_slot_type(slot_index);
      if (slot_type == RACE_SLOT_TYPE_HUMAN) {
        leader_slot = slot_index;
      }
      slot_index = slot_index + 1;
    } while (slot_index < RACE_SLOT_COUNT);
    /* a human slot was found: measure its distance */
    if (leader_slot != NO_LEADER_SLOT) {
      leader_dist = (double)player_distance_get(leader_slot);
      compare_value = (double)k_f_2_5;
      bot->leader_distance = (float)leader_dist;
      /* leader is further than k_f_2_5: add to the far time, return 1 past the timeout */
      if ((compare_value < leader_dist) &&
         (leader_slot = bot->leader_far_ms + g_tick_count,
         bot->leader_far_ms = leader_slot, LEADER_FAR_TIMEOUT_MS < leader_slot)) {
        return 1;
      }
      /* leader distance at or above float_1_0: return 0 and keep the far time.
         Below float_1_0: fall through and reset the far time. */
      if ((double)float_1_0 <= leader_dist) {
        return 0;
      }
      bot->leader_far_ms = 0;
      return 0;
    }
  }
  /* no human slot, or bot distance is not 0: reset the far time */
  bot->leader_far_ms = 0;
  return 0;
}


