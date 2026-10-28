// cluster-entries: voice_set_volume
/* Installed cluster voice_set_volume (tools/cluster_install.py from output/cluster_pilot_zy/voice_set_volume.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*006ad210*/ extern VoiceSlot g_voices[];
/*005b7300*/ extern void sound_voice_set_float_param(int voice, int param, int enable, float value);

/* Sets the volume (clamped to 0..1) of voice slot `index` in the slot table and on its sound object. */
void voice_set_volume(int index, float volume)
{
  if (volume < 0.0f) {      /* NaN passes unchanged (original: fcomp / test ah,1 / jne ...) */
    volume = 0.0f;
  } else if (volume > 1.0f) {
    volume = 1.0f;
  }
  *(float *)(g_voices[index].voice + 0x40) = volume;
  if (*(int *)(g_voices[index].voice + 0x30) != 0) {
    sound_voice_set_float_param(*(int *)(g_voices[index].voice + 0x30), 5, 1, volume);
  }
  g_voices[index].volume = volume;
}

