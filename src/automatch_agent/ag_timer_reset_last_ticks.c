#include "mfl_tier2.h"
/*007ef818*/ extern undefined4 g_last_frame_ticks;
/* timer_reset_last_ticks: same machine code as race_setup_use_0 (tools/propagate_dups.py), byte gate checked */
void timer_reset_last_ticks(void)
{
  g_last_frame_ticks = timer_ticks();
  return;
}
