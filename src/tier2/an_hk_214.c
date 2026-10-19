#include "mfl_tier2.h"
/*007f0284*/ extern struct AiData g_ai_data;
/*008c998c*/ extern undefined4 g_car_8c998c;
/*008c9ae0*/ extern struct AiBotRecord g_ai_bots[4];
/*008c9cf0*/ extern char g_ai_bots_8c9cf0[];
#define AI_BOT_RECORD_WORDS (0x1e) /* sizeof(struct AiBotRecord) / 4 */
#define AI_BOT_STATE_STRIDE (0x14) /* bytes per bot in g_ai_data.bot_state */
/* ai_bots_reset */

/* "clears the bot_state of g_ai_data and the per bot counters at 0x8C9B10" */
/* Resets the AI bots for a new race: clears g_car_8c998c and the counters and
   state of all 4 g_ai_bots records (fields from field_0x30 onwards, the
   catch-up / stage / weapon timers). Each bot's bot_state entry is set to 1000
   (0x3e8, meaning inferred: a timer in ms). No parameters, no return value. */

void ai_bots_reset(void)

{
  undefined4 *bot_ptr;
  byte *bot_state_entry;
  
  g_car_8c998c = 0;
  bot_state_entry = g_ai_data.bot_state + 4;
  bot_ptr = (undefined4 *)&g_ai_bots[0].field_0x30;
  /* One pass per bot: bot_ptr points at field_0x30 of the current record
     (the record base is bot_ptr - 0x30 bytes); bot_state_entry at its 20 byte slot. */
  do {
    bot_ptr[-1] = 0;
    *bot_ptr = 0;
    bot_ptr[1] = 0;
    bot_ptr[-0xb] = 0;
    bot_ptr[-10] = 0;
    bot_ptr[-9] = 0;
    bot_ptr[-8] = 0;
    bot_ptr[7] = 0;
    bot_ptr[0xc] = 0;
    bot_ptr[0xd] = 0;
    bot_ptr[0xe] = 0;
    bot_ptr[0xf] = 0;
    bot_state_entry[0] = 0xe8;
    bot_state_entry[1] = 3;
    bot_state_entry[2] = 0;
    bot_state_entry[3] = 0;
    bot_ptr = bot_ptr + AI_BOT_RECORD_WORDS;
    bot_state_entry = bot_state_entry + AI_BOT_STATE_STRIDE;
  } while ((LONG_PTR)bot_ptr < ((LONG_PTR)&g_ai_bots_8c9cf0));
  return;
}


