// cluster-entries: sound_master_fade_to_default
/* Installed cluster sound_master_fade_to_default (tools/cluster_install.py from output/cluster_pilot_zy/sound_master_fade_to_default.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*00617954*/ extern float g_sound_617954;
/*00469f40*/ extern void sound_master_fade_towards(float target);

/* Fades the master sound volume towards its default (1.0): the current value while it is below 1.0, else 1.0. */
void sound_master_fade_to_default(void)
{
  if (g_sound_617954 < 1.0f) {
    sound_master_fade_towards(g_sound_617954);
    return;
  }
  sound_master_fade_towards(1.0f);
}

