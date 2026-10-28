// cluster-entries: sound_sfx_update_position_volume
/* Installed cluster sound_sfx_update_position_volume (tools/cluster_install.py from output/cluster_pilot_zy/sound_sfx_update_position_volume.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*006ad080*/ extern int g_sound_mute_when_multiplayer;
/*008d98bc*/ extern int g_sound_player_count;
/*004a20c0*/ extern int get_app_state(void);
/*006187f8*/ extern int g_sfx_table[];
/*006ad210*/ extern VoiceSlot g_voices[];
/*005b72a0*/ extern void sound_voice_call_entry(int voice, int param, int enable, float *position);
/*005b7300*/ extern void sound_voice_set_float_param(int voice, int param, int enable, float value);
/*0046a0c0*/ extern void voice_set_volume(int index, float volume);

/* Updates a playing sound: position (3 floats copied to voice+0x58), volume clamped to 0..1 and pitch/rate (voice+0x3c).  Skipped for sound 0x20 in split-screen multiplayer when muting is on. */
void sound_sfx_update_position_volume(int sfx, float *position, int offset, float volume, float rate)
{
  int slot;

  if (g_sound_mute_when_multiplayer != 0 && g_sound_player_count > 1 && sfx == 0x20) {
    return;
  }
  if (get_app_state() != APP_STATE_RACE) {
    return;
  }
  slot = g_sfx_table[sfx * 0x25] + offset;
  if ((*(unsigned char *)(g_voices[slot].voice + 0x34) & 2) == 0) {
    return;
  }
  *(float *)(g_voices[slot].voice + 0x58) = position[0];
  *(float *)(g_voices[slot].voice + 0x5c) = position[1];
  *(float *)(g_voices[slot].voice + 0x60) = position[2];
  if (*(int *)(g_voices[slot].voice + 0x30) != 0) {
    sound_voice_call_entry(*(int *)(g_voices[slot].voice + 0x30), 0, 1, position);
  }
  if (volume > 1.0f) {
    volume = 1.0f;
  } else if (volume < 0.0f) {
    volume = 0.0f;
  }
  voice_set_volume(slot, volume);
  *(float *)(g_voices[slot].voice + 0x3c) = rate;
  if (*(int *)(g_voices[slot].voice + 0x30) != 0) {
    sound_voice_set_float_param(*(int *)(g_voices[slot].voice + 0x30), 4, 1, rate);
  }
}

