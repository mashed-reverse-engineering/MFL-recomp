// cluster-entries: sound_master_fade_towards
/* Installed cluster sound_master_fade_towards (tools/cluster_install.py from output/cluster_pilot_k/sound_master_fade_towards.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*005ddb00*/ extern float k_f_0_025;
/*00617948*/ extern float g_sound_master_fade;
/* sound_master_fade_towards */

/* "(target): g_sound_617948 moves by 0.025 per call towards target" */

void sound_master_fade_towards(float target)

{
  if (g_sound_master_fade < target) {
    g_sound_master_fade = g_sound_master_fade + k_f_0_025;
  }
  if (target < g_sound_master_fade) {
    g_sound_master_fade = g_sound_master_fade - k_f_0_025;
  }
  return;
}

