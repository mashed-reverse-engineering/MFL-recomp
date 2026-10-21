#include "mfl_tier2.h"
extern struct VoiceSlot g_voices[210];
/*0047b520*/ extern int car_current_matrix();
/*005b72a0*/ extern int FUN_005b72a0(undefined4 p0, undefined4 p1, undefined4 p2, void *p3);
/* voice_set_position_from_car */

/* "(voice, car): voice position = the car's current matrix position (12 callers)" */

void voice_set_position_from_car(int slot, undefined4 car)
{
  char *voice;
  char *matrix;
  int sound;

  car_current_matrix(&matrix, car);
  voice = *(char **)&g_voices[slot].voice;
  *(undefined4 *)(voice + 0x58) = *(undefined4 *)(matrix + 0x30);
  *(undefined4 *)(voice + 0x5c) = *(undefined4 *)(matrix + 0x34);
  *(undefined4 *)(voice + 0x60) = *(undefined4 *)(matrix + 0x38);
  sound = *(int *)(*(char **)&g_voices[slot].voice + 0x30);
  if (sound != 0) {
    FUN_005b72a0(sound, 0, 1, matrix + 0x30);
  }
}
