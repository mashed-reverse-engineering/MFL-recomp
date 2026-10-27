#include "mfl_tier2.h"
/*008c9ae0*/ extern struct AiBotRecord g_ai_bots[4];


/* ai_bot_get_flag_20 */

/* "0x8C9B20[bot * 0x78]" */

/* Read the word at struct offset 0x40 of AI bot bot_index (the name's "20" does not match the offset). */
undefined4 __cdecl ai_bot_get_flag_20(int bot_index)
{
  return *(undefined4 *)&g_ai_bots[bot_index].field_0x40;
}
