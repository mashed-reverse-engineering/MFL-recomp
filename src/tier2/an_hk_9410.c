#include "mfl_tier2.h"
extern struct VoiceSlot g_voices[210];
/*0047b520*/ extern int car_current_matrix();
/* layout of *voice in voice_set_position_from_car, reconstructed from the byte offsets it is accessed at (explicit padding: the offsets are the original ones, every field sits at its natural alignment; fields are named by offset) */
typedef struct voice_set_position_from_car_voice_layout {
    char pad_0x0[88];
    undefined4 f_0x58;
    undefined4 f_0x5c;
    undefined4 f_0x60;
} voice_set_position_from_car_voice_layout;

/*005b72a0*/ extern int sound_voice_call_entry(undefined4 p0, undefined4 p1, undefined4 p2, void *p3);
/* voice_set_position_from_car */

/* "(voice, car): voice position = the car's current matrix position (12 callers)" */

/* TIER2: validated 2026-10-21 authority=diffemu */
void voice_set_position_from_car(int slot, undefined4 car)
{
  voice_set_position_from_car_voice_layout *voice;
  char *matrix;
  int sound;

  car_current_matrix(&matrix, car);
  voice = (voice_set_position_from_car_voice_layout *)(*(char **)&g_voices[slot].voice);
  voice->f_0x58 = *(undefined4 *)(matrix + 0x30);
  voice->f_0x5c = *(undefined4 *)(matrix + 0x34);
  voice->f_0x60 = *(undefined4 *)(matrix + 0x38);
  sound = *(int *)(*(char **)&g_voices[slot].voice + 0x30);
  if (sound != 0) {
    sound_voice_call_entry(sound, 0, 1, matrix + 0x30);
  }
}
