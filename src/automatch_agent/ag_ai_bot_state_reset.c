#include "mfl_tier2.h"
/*007f0284*/ extern struct AiData g_ai_data;
/*00801b0c*/ extern char g_car_801b0c[];

#define ALL_BOTS -1
#define BOT_STATE_DEFAULT 1000   /* 0x3E8 */
#define BOT_STATE_STRIDE 5       /* ints per bot record */
#define BOT_STATE_END ((int)&g_car_801b0c)   /* end address of the bot state table */

/* ai_bot_state_reset */

/* Resets one bot's state field (or all bots when bot_index == ALL_BOTS) to BOT_STATE_DEFAULT. */

void __cdecl ai_bot_state_reset(int bot_index)

{
  int *state_field;

  if (bot_index == ALL_BOTS) {
    /* walk every bot record until the end of the table */
    state_field = (int *)(g_ai_data.bot_state + 4);
    do {
      *state_field = BOT_STATE_DEFAULT;
      state_field = state_field + BOT_STATE_STRIDE;
    } while ((int)state_field < BOT_STATE_END);
    return;
  }
  ((int *)(g_ai_data.bot_state + 4))[bot_index * BOT_STATE_STRIDE] = BOT_STATE_DEFAULT;
  return;
}
