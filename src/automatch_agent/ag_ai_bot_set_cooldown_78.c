#include "mfl_tier2.h"
/*008c9980*/ extern float g_ai_skill;
/*008c9ae0*/ extern struct AiBotRecord g_ai_bots[4];

#define COOLDOWN_BASE_FRAMES 0x78   /* 120: cooldown for skill 10 and below */

/* ai_bot_set_cooldown_78 */

/* Sets a bot's cooldown from skill: 120 - 120 * (skill - 10), then flags it. */

void ai_bot_set_cooldown_78(int bot_index)

{
  int skill_steps;

  skill_steps = (int)(g_ai_skill - 10.0f);
  *(int *)&g_ai_bots[bot_index].field_0x3c = skill_steps * -COOLDOWN_BASE_FRAMES + COOLDOWN_BASE_FRAMES;
  *(undefined4 *)&g_ai_bots[bot_index].field_0x40 = 1;
  return;
}
