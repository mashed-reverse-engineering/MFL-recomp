// cluster-entries: sound_music_fade_update
/* Installed cluster sound_music_fade_update (tools/cluster_install.py from output/cluster_pilot_zy/sound_music_fade_update.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*0061a168*/ extern int g_sound_music_voice;
/*006ad8b4*/ extern float g_sound_frame_6ad8b4;
/*006adcf4*/ extern float g_sound_frame_6adcf4;
/*006ad8ac*/ extern float g_sound_music_fade;
/*006ad210*/ extern VoiceSlot g_voices[];
/*005b6a40*/ extern void FUN_005b6a40(int voice, int enable);
/*005b7300*/ extern void FUN_005b7300(int voice, int param, int enable, float value);
/*0046a0c0*/ extern void voice_set_volume(int index, float volume);

/* Music fade: volume = frame factor * fade, clamped to 0..1, applied to the music voice (silenced and stopped when the volume is not above 0). */
void sound_music_fade_update(void)
{
  int music = g_sound_music_voice;

  g_sound_frame_6ad8b4 = g_sound_frame_6adcf4 * g_sound_music_fade;
  if (g_sound_frame_6ad8b4 <= 0.0f) {
    FUN_005b6a40(g_voices[music].voice, 0);
    return;
  }
  if ((*(unsigned char *)(g_voices[music].voice + 0x34) & 2) == 0) {
    FUN_005b6a40(g_voices[music].voice, 1);
  }
  *(float *)(g_voices[music].voice + 0x3c) = 22050.0f;
  if (*(int *)(g_voices[music].voice + 0x30) != 0) {
    FUN_005b7300(*(int *)(g_voices[music].voice + 0x30), 4, 1, 22050.0f);
  }
  if (g_sound_frame_6ad8b4 > 1.0f) {
    g_sound_frame_6ad8b4 = 1.0f;
    voice_set_volume(music, g_sound_frame_6ad8b4);
    return;
  }
  if (g_sound_frame_6ad8b4 < 0.0f) {
    g_sound_frame_6ad8b4 = 0.0f;
  }
  voice_set_volume(music, g_sound_frame_6ad8b4);
}

