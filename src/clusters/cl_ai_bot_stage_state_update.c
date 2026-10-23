// cluster-entries: ai_bot_stage_state_update
/* Installed cluster ai_bot_stage_state_update (tools/cluster_install.py from output/cluster_pilot_uc3/ai_bot_stage_state_update.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*00434bb0*/ extern int get_event_type(void);
/*00437f10*/ extern int training_lesson_get(void);
/*00416ac0*/ extern void ai_bot_state_reset(int bot);
/*00481fd0*/ extern double random_range(float low, float high);
/*008c9ae0*/ extern struct AiBotRecord g_ai_bots[4];
/*007f0284*/ extern struct AiData g_ai_data;
extern int g_ai_catchup_class;
extern int g_ai_bot_6582b4;
extern int g_race_clock_b;
extern int g_tick_count;

/* Length of the route `slot` (0..2) of route set `set` (0 race, 1 inside, 2 slow, 3 cheat lines): the last int of its 0x204 byte record. */
#define ROUTE_LENGTH(set, slot) (*(int *)(g_ai_data.routes + ((set) * 3 + (slot)) * 0x204 + 0x200))

/* Advances the AI stage state machine of bot `bot` (stage 0 -> 1 -> 2, 3 -> 0; stage_counter picks the route slot 0..2): while the bot is active its stage
   counter steps through the three routes of the stage's route set and moves on to the next stage when the next route is shorter than 4 or all three
   were used.  Afterwards the activity timer runs (flag_48 counts up to 9000 ms), then the stage is forced by the catch-up class and every 3000 ms tick
   with (tick & 15) == 15 the counter may advance randomly once. */
void ai_bot_stage_state_update(int bot)
{
  AiBotRecord *b;
  int stage;
  int next;
  int tick;

  if (get_event_type() == EVENT_TRAINING) {
    if (training_lesson_get() == 0) {
      g_ai_bots[bot].stage = 0;
      g_ai_bots[bot].stage_counter = 0;
      return;
    }
    if (training_lesson_get() == 5) {
      g_ai_bots[bot].stage = 0;
      if (bot == 0) {
        g_ai_bots[0].stage_counter = 0;
        return;
      }
      if (bot == 1) {
        g_ai_bots[1].stage_counter = 1;
        return;
      }
      if (bot == 2) {
        g_ai_bots[2].stage_counter = 2;
      }
      return;
    }
  }
  b = &g_ai_bots[bot];
  if (b->active != 0) {
    b->flag_48 = 1;
    stage = b->stage;
    if (stage == 2) {
      b->stage_counter = b->stage_counter + 1;
      if (b->stage_counter > 2) {
        b->stage_counter = 0;
      }
      ai_bot_state_reset(-1);
    }
    if (stage == 1) {
      next = b->stage_counter + 1;
      b->stage_counter = next;
      if (next > 2 || ROUTE_LENGTH(1, next) < 4) {
        b->stage = 2;
        b->stage_counter = 0;
      }
    }
    if (b->stage == 0) {
      next = b->stage_counter + 1;
      b->stage_counter = next;
      if (next > 2 || ROUTE_LENGTH(0, next) < 4) {
        b->stage = 1;
        b->stage_counter = 0;
      }
      ai_bot_state_reset(-1);
    }
    if (b->stage == 3) {
      next = b->stage_counter + 1;
      b->stage_counter = next;
      if (next > 2 || ROUTE_LENGTH(3, next) < 4) {
        b->stage = 0;
        b->stage_counter = 0;
      }
      ai_bot_state_reset(-1);
    }
    b->active = 0;
  }
  if (b->flag_48 != 0 && b->flag_48 < 9000) {
    b->flag_48 = b->flag_48 + g_tick_count;
    return;
  }
  b->flag_48 = 0;
  if (g_ai_catchup_class == 0) {
    b->stage = 0;
  }
  if (g_ai_catchup_class == 2) {
    b->stage = 0;
  }
  if (g_ai_catchup_class == 1) {
    if (b->stage != 2) {
      ai_bot_state_reset(-1);
    }
    b->stage = 2;
  }
  tick = g_race_clock_b / 3000;
  if ((tick & 0xf) != 0xf) {
    g_ai_bot_6582b4 = 0;
    return;
  }
  if (g_ai_bot_6582b4 != tick && random_range(0.0f, 1.0f) < 0.5f) {
    b->stage_counter = b->stage_counter + 1;
    ai_bot_state_reset(-1);
  }
  g_ai_bot_6582b4 = tick;
}

