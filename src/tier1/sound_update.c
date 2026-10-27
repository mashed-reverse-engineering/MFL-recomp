#include "mfl_tier2.h"
/*0046a010*/ extern int sound_stop_all_voices_and_streams();
/*00473450*/ extern int sound_stop_all_and_release();


/* sound_update */

/* "sound_fn_46a010 (race sounds) + sound_fn_473450 (voice update)" */

void sound_update(void)
{
  sound_stop_all_voices_and_streams();
  sound_stop_all_and_release();
  return;
}
