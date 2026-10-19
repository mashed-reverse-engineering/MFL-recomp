#include "mfl_tier2.h"
/*004b2b48*/ extern int _ftol();
/*008c9ae0*/ extern struct AiBotRecord g_ai_bots[4];
/*008c9980*/ extern float g_ai_skill;

#define AI_COOLDOWN_BASE (0xb4)
#define AI_SKILL_REFERENCE (1e+01f)

/* ai_bot_set_cooldown_b4 */

/* "0x8C9B1C[bot] = 0xB4 - 0xB4 * ftol(x)" */

/*
 * Sets the cooldown timer (ms countdown, see cooldown_timer in AiBotRecord) of AI bot `bot_index`
 * from the global AI skill: the timer starts at AI_COOLDOWN_BASE and drops by AI_COOLDOWN_BASE
 * for every whole point the skill is above AI_SKILL_REFERENCE (inferred: higher skill = shorter cooldown).
 */
void __cdecl ai_bot_set_cooldown_b4(int bot_index)

{
  int skill_offset;

  skill_offset = ((int)(g_ai_skill - AI_SKILL_REFERENCE));
  g_ai_bots[bot_index].cooldown_timer = skill_offset * -AI_COOLDOWN_BASE + AI_COOLDOWN_BASE;
  return;
}


