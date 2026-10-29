#include "mfl_tier2.h"
/*00413d10*/ extern void replay_save_00413d10(void);

/* Jump stub at 0x40f800: forwards to the replay save routine. */
void
replay_save_thunk_40f800(void)
{
  replay_save_00413d10();
}
