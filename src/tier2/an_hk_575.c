#include "mfl_tier2.h"
/* typed view of VoiceSlot (header: uint voice): record pointer as a real pointer, same 8 byte layout on x86 */
struct VoiceSlotP { char *voice; float volume; };
extern struct VoiceSlotP g_voices[210];
/*00469ac0*/ extern int voice_set_position_from_car();
/*0046a0c0*/ extern int voice_set_volume(int p1, float p2);
/*0047ac30*/ extern int car_get_mode();
/*005b6a40*/ extern int FUN_005b6a40(undefined4 p0, undefined4 p1);
/*005b7300*/ extern int FUN_005b7300(undefined4 p0, undefined4 p1, undefined4 p2, float p3);
/*00618c98*/ extern undefined4 g_sound_618c98;
#define CAR_MODE_VOICE (2) /* car mode from car_get_mode (inferred) */
#define VOICE_FLAGS_OFFSET (0x34)
#define VOICE_HANDLE_OFFSET (0x30)
#define VOICE_INIT_OFFSET (0x3c)
#define VOICE_INIT_VALUE (0x46ac4400) /* float bit pattern (inferred) */
#define VOICE_INIT_VALUE_F (22050.0f) /* the same value as a float argument of FUN_005b7300 (`float p3`; the integer literal would convert to 1.18e9) */
#define VOICE_MODE2_VOLUME (0.7)
/* sound_car_mode_2_voice */

/* "starts the voice when the car mode is 2 and count > 0, volume 0.7" */

/*
 * Sound voice update for one car in car mode 2 (voice mode, inferred).
 * car_index: index of the car; also used as the base into g_voices (offset by g_sound_618c98).
 * Note: car_get_mode writes the car mode back into car_index, so the mode check below reads it.
 * Returns nothing.
 */
void sound_car_mode_2_voice(int car_index)

{
  int voice_slot;
  int iVar2;
  undefined4 handle_result;
  int mode_count;

  iVar2 = car_index;
  voice_slot = g_sound_618c98 + car_index;
  /* read car mode and count; voice is only started in mode 2 with count > 0 */
  car_get_mode(car_index,&car_index,&mode_count);
  if ((car_index == CAR_MODE_VOICE) && (0 < mode_count)) {
    /* only (re)start the voice when flag bit 2 is clear (inferred: not already playing) */
    if ((*(byte *)(g_voices[voice_slot].voice + VOICE_FLAGS_OFFSET) & 2) == 0) {
      voice_set_position_from_car(voice_slot,iVar2);
      FUN_005b6a40(g_voices[voice_slot].voice,1);
      *(undefined4 *)(g_voices[voice_slot].voice + VOICE_INIT_OFFSET) = VOICE_INIT_VALUE;
      iVar2 = *(int *)(g_voices[voice_slot].voice + VOICE_HANDLE_OFFSET);
      handle_result = 0;
      if (iVar2 != 0) {
        handle_result = FUN_005b7300(iVar2,4,1,VOICE_INIT_VALUE_F);
      }
      voice_set_volume(voice_slot,VOICE_MODE2_VOLUME);
      return;
    }
  }
  else {
    voice_set_volume(voice_slot,0.0);
  }
  return;
}


