#include "mfl_tier2.h"
/* typed view of VoiceSlot (header: uint voice): record pointer as a real pointer, same 8 byte layout on x86 */
struct VoiceSlotP { char *voice; float volume; };
extern struct VoiceSlotP g_voices[210];
/*00469ac0*/ extern int voice_set_position_from_car();
/*0046a0c0*/ extern int voice_set_volume(int p1, float p2);
/*004a20c0*/ extern int get_app_state();
/*005b6a40*/ extern int FUN_005b6a40(undefined4 p0, undefined4 p1);
/*005b7300*/ extern int FUN_005b7300(undefined4 p0, undefined4 p1, undefined4 p2, float p3);
/*006187f8*/ extern undefined4 g_sfx_table;

/* voice record offsets, in bytes from the voice pointer in g_voices[].voice */
#define VOICE_OFF_FLAGS (0x34)
#define VOICE_OFF_HANDLE (0x30)
#define VOICE_OFF_FREQ (0x3c)
/* bit in the voice flags: set = voice busy, sfx not started (inferred) */
#define VOICE_FLAG_BUSY (2)
/* voice slots per g_sfx_table entry (inferred) */
#define SFX_BLOCK_SIZE (0x25)
/* float bit pattern 22050.0f (playback frequency) stored at VOICE_OFF_FREQ and passed as p3 (inferred) */
#define SFX_CAR_FREQ (0x46ac4400)
#define SFX_CAR_FREQ_F (22050.0f) /* the same value as a float argument of FUN_005b7300 (`float p3`; the integer literal would convert to 1.18e9) */

/* sound_sfx_play_at_car */

/* "starts sfx table entry param_1 for car param_2 with position and volume 1"
 *
 * Starts sfx `sfx_id` on the voice slot of car `car_index`, but only when the app
 * state is 3 (in race) and that voice is not already busy. The voice is positioned
 * at the car, its frequency is set to SFX_CAR_FREQ, and its volume is set to 1.0.
 * Parameters: sfx_id = index into g_sfx_table (the sfx entry), car_index = car number.
 * Returns nothing.
 */

void __cdecl sound_sfx_play_at_car(int sfx_id,int car_index)

{
  int voice_handle;
  int voice_slot;
  undefined4 sfx_result;

  /* app_state is read first; the same local is then reused as the voice slot index */
  voice_slot = get_app_state();
  /* only play in state 3 (in race) and when the car's voice is not busy */
  if ((voice_slot == 3) &&
     (voice_slot = (&g_sfx_table)[sfx_id * SFX_BLOCK_SIZE] + car_index,
     (*(byte *)(g_voices[voice_slot].voice + VOICE_OFF_FLAGS) & VOICE_FLAG_BUSY) == 0)) {
    voice_set_position_from_car(voice_slot,car_index);
    FUN_005b6a40(g_voices[voice_slot].voice,1);
    *(undefined4 *)(g_voices[voice_slot].voice + VOICE_OFF_FREQ) = SFX_CAR_FREQ;
    voice_handle = *(int *)(g_voices[voice_slot].voice + VOICE_OFF_HANDLE);
    sfx_result = 0;
    if (voice_handle != 0) {
      sfx_result = FUN_005b7300(voice_handle,4,1,SFX_CAR_FREQ_F);
    }
    voice_set_volume(voice_slot,1.0);
  }
  return;
}


