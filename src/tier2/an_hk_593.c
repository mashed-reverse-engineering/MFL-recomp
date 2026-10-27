#include "mfl_tier2.h"
/*00416a30*/ extern int career_cell_index();
/*00434bb0*/ extern int get_event_type();
/*00435410*/ extern int menu_bot_skill_b_get();
/*004379b0*/ extern int menu_bot_skill_a_get();
/*00437f10*/ extern int training_lesson_get();
/*005dd340*/ extern float k_f_0_5;
/*005dd620*/ extern float float_10_0;
/*005ddb58*/ extern float k_f_1000;
/*005ddc28*/ extern float k_f_0;
/*005de20c*/ extern float k_f_2_5;
/*005de210*/ extern float k_f_9;
/*007ef770*/ extern float g_ai_bot_7ef770;
/*008c9980*/ extern float g_ai_skill;
/*008c9984*/ extern undefined4 g_ai_last_active_player;
/*008c999c*/ extern float g_ai_skill_bias;
/*008c99a4*/ extern undefined4 g_ai_career_skill_table;
/*008c9eb0*/ extern float g_ai_leader_slots;
/*008c9eb4*/ extern float g_car_8c9eb4;
/*008c9eb8*/ extern float g_car_8c9eb8;
/*008c9ebc*/ extern float g_car_8c9ebc;

#define BOT_SKILL_MAX (10.0)
#define TRAINING_BOT_SKILL (5.0)
#define AI_NO_ACTIVE_PLAYER (0xffffffff)
/* ai_bot_skill */

/* "(bot): skill value from the 0x8C99A4 table by career cell (9.0 in event types 0xE..0x10)" */

/*
 * Computes the AI bot skill for the current event and stores it in g_ai_skill (inferred: the
 * AI difficulty used by the bots), then resets the leader/car slot globals to -1 (unset).
 * Takes no parameters and returns nothing.
 * Base value: career table entry for career_cell_index(), plus g_ai_skill_bias * 0.5.
 * Overrides by event type: 0xE..0x10 -> 9.0; 6..8 -> menu_bot_skill_a * 2.5; 0xB..0xD -> menu_bot_skill_b * 2.5.
 * Result is clamped to 0 (below k_f_0) or BOT_SKILL_MAX (above it), then training (EVENT_TRAINING)
 * stores 9.0 when training_lesson_get() == 5 and TRAINING_BOT_SKILL otherwise.
 */
void __cdecl ai_bot_skill(void)

{
  int event_type;
  float bot_skill;
  
  /* base skill from the career table, cell chosen by the career position */
  event_type = career_cell_index();
  bot_skill = (float)(int)(&g_ai_career_skill_table)[event_type];
  /* event types 0xE..0x10 always use 9.0 */
  event_type = get_event_type();
  if (((event_type == EVENT_TURBO_TOURNAMENT) || (event_type = get_event_type(), event_type == 0xf)) ||
     (event_type = get_event_type(), event_type == 0x10)) {
    bot_skill = k_f_9;
  }
  bot_skill = g_ai_skill_bias * k_f_0_5 + bot_skill;
  /* event types 6..8: skill from menu_bot_skill_a, scaled by 2.5 */
  event_type = get_event_type();
  if (((event_type == EVENT_MULTIPLAYER_6) || (event_type = get_event_type(), event_type == 7)) ||
     (event_type = get_event_type(), event_type == 8)) {
    event_type = menu_bot_skill_a_get();
    bot_skill = (float)event_type * k_f_2_5;
  }
  /* event types 0xB..0xD: skill from menu_bot_skill_b, scaled by 2.5 */
  event_type = get_event_type();
  if (((event_type == EVENT_QUICK_RACE_11) || (event_type = get_event_type(), event_type == 0xc)) ||
     (event_type = get_event_type(), event_type == 0xd)) {
    event_type = menu_bot_skill_b_get();
    bot_skill = (float)event_type * k_f_2_5;
  }
  /* clamp to [0, BOT_SKILL_MAX] */
  if (bot_skill < k_f_0) {      /* NaN passes both clamps unchanged (original: fcomp, test ah,5, jp / test ah,0x41, jne) */
    bot_skill = 0.0;
  }
  else if (float_10_0 < bot_skill) {
    bot_skill = BOT_SKILL_MAX;
  }
  /* training: lesson 5 keeps 9.0, other lessons use TRAINING_BOT_SKILL */
  event_type = get_event_type();
  if (event_type == EVENT_TRAINING) {
    bot_skill = TRAINING_BOT_SKILL;
    event_type = training_lesson_get();
    g_ai_skill = k_f_9;
    if (event_type == 5) goto LAB_00416c05;
  }
  g_ai_skill = bot_skill;
LAB_00416c05:
  g_ai_leader_slots = (-1.0f);
  g_car_8c9eb4 = (-1.0f);
  g_car_8c9eb8 = (-1.0f);
  g_ai_bot_7ef770 = g_ai_skill + k_f_1000;
  g_ai_last_active_player = AI_NO_ACTIVE_PLAYER;
  g_car_8c9ebc = (-1.0f);
  return;
}


