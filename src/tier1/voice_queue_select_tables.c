#include "mfl_tier2.h"
/*008bd7bc*/ extern undefined4 g_weapon_ai_8bd7bc;
/*008bd7c0*/ extern undefined g_weapon_ai_8bd7c0;
/*008bdcc0*/ extern undefined4 g_weapon_ai_8bdcc0;
/*008bdce0*/ extern undefined g_weapon_ai_8bdce0;
/*008be1f0*/ extern undefined4 g_weapon_ai_8be1f0;
/*008be220*/ extern undefined g_weapon_ai_8be220;
/*008be7f0*/ extern undefined4 g_weapon_ai_8be7f0;
/*008be800*/ extern undefined g_weapon_ai_8be800;
/*008bed04*/ extern undefined4 g_voice_queue_count_ptr;
/*008bed08*/ extern undefined4 g_voice_queue_entries_ptr;
/* voice_queue_select_tables */
/* "(team 1..3, else 0): selects the queue count / queue entry tables of the team (0x8BED04 /
   0x8BED08)" */
/* Points g_voice_queue_count_ptr (0x8BED04) and g_voice_queue_entries_ptr (0x8BED08) at the
   voice queue tables of `team` (1..3). Any other team value takes the fallback pair (inferred:
   the default for no team). Returns nothing. */
void voice_queue_select_tables(int team)
{
  switch (team) {
  case 1:
    g_voice_queue_count_ptr = &g_weapon_ai_8bd7bc;
    g_voice_queue_entries_ptr = &g_weapon_ai_8bd7c0;
    return;
  case 2:
    g_voice_queue_count_ptr = &g_weapon_ai_8be1f0;
    g_voice_queue_entries_ptr = &g_weapon_ai_8be220;
    return;
  case 3:
    g_voice_queue_count_ptr = &g_weapon_ai_8bdcc0;
    g_voice_queue_entries_ptr = &g_weapon_ai_8be800;
    return;
  default:
    g_voice_queue_count_ptr = &g_weapon_ai_8be7f0;
    g_voice_queue_entries_ptr = &g_weapon_ai_8bdce0;
    return;
  }
}
