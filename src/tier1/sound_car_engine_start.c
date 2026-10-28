#include "mfl_tier2.h"
/* typed view of VoiceSlot (header: uint voice): record pointer as a real pointer, same 8 byte layout on x86 */
struct VoiceSlotP { char *voice; float volume; };
extern struct VoiceSlotP g_voices[210];
/*0040fdc0*/ extern int race_get_phase();
/*00469ac0*/ extern int voice_set_position_from_car();
/*0046a0c0*/ extern int voice_set_volume(int p1, float p2);
/*0047ac90*/ extern int car_get_int_from_float_field();
/*0047b750*/ extern int car_get_speed();
/*005b6a40*/ extern int sound_voice_set_enabled(void *p0, undefined4 p1);
/*005b7300*/ extern int sound_voice_set_float_param(undefined4 p0, undefined4 p1, undefined4 p2, float p3);
/*0061a040*/ extern undefined4 g_sound_frame_61a040;
/*006ad080*/ extern undefined4 g_sound_mute_when_multiplayer;
/*006ae130*/ extern undefined g_sound_6ae130;
/*008d98bc*/ extern int g_sound_player_count;
/* sound_car_engine_start */
/* Engine sound for one car: when the car is "pending start" (g_sound_6ae130[car]), copy its
   speed/int field into the voice slot, set the voice's position, pitch and volume, then clear
   the pending flag. Does nothing while multiplayer muting is on with 2+ players.
   Parameter car_index: the car to update (0-based index into the per-car arrays).
   Returns nothing. */
void sound_car_engine_start(int car_index)
{
  int voice_slot;
  int race_phase;
  float engine_volume;
  struct {
    int car_int_field;
    undefined1 speed_out [4];
  } frame;

  /* skip entirely if sound is muted in multiplayer and there are 2+ players */
  if ((g_sound_mute_when_multiplayer == 0) || (g_sound_player_count <= 1)) {
    voice_slot = g_sound_frame_61a040 + car_index;
    voice_set_volume(voice_slot,0.0);
    /* only cars with the pending-start flag set continue */
    if (((int *)&g_sound_6ae130)[car_index] != 0) {
      car_get_speed(frame.speed_out,car_index);
      car_get_int_from_float_field(&frame.car_int_field,car_index);
      /* only start the engine voice when the car's int field is positive (inferred: speed/gear) */
      if (frame.car_int_field >= 1) {
        voice_set_position_from_car(voice_slot,car_index);
        /* make sure the voice record's flag bit 1 is set (inferred: voice is active) */
        if ((*(byte *)(g_voices[voice_slot].voice + 0x34) & 2) == 0) {
          sound_voice_set_enabled(g_voices[voice_slot].voice,1);
        }
        *(undefined4 *)(g_voices[voice_slot].voice + 0x3c) = 0x4694d400;
        /* the record's +0x30 pointer is forwarded to sound_voice_set_float_param when present */
        race_phase = *(int *)(g_voices[voice_slot].voice + 0x30);
        if (race_phase != 0) {
          sound_voice_set_float_param(race_phase,4,1,19050.0f);
        }
        /* volume is 0.45 in race phase 6, silent otherwise (inferred: phase 6 = racing) */
        race_phase = race_get_phase();
        if (race_phase != 6) {
          voice_set_volume(voice_slot,0.0f);
        }
        else {
          voice_set_volume(voice_slot,0.45f);
        }
        /* clear the pending-start flag for this car */
        ((undefined4 *)&g_sound_6ae130)[car_index] = 0;
      }
    }
  }
  return;
}
