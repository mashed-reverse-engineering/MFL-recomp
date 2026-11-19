#include "mfl_tier2.h"
/*008c9ec0*/ extern undefined4 g_ai_player_counters;
/* ai_get_counter: same machine code as time_get_fraction (tools/propagate_dups.py), byte gate checked */
double ai_get_counter(int param_1)
{
  return (double)((float *)&g_ai_player_counters)[param_1];
}
