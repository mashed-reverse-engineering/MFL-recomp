#include "mfl_tier2.h"
/* typed view of VoiceSlot (header: uint voice): record pointer as a real pointer, same 8 byte layout on x86 */
struct VoiceSlotP { char *voice; float volume; };
extern struct VoiceSlotP g_voices[210];
/*0042b110*/ extern int current_course_id(void);
/*0046a0c0*/ extern int voice_set_volume(int p1, float p2);
/*004a20c0*/ extern int get_app_state(void);
/*00481fd0*/ extern double random_range();
/*005b6a40*/ extern int FUN_005b6a40(void *p0, undefined4 p1);
/*005b72a0*/ extern int FUN_005b72a0(undefined4 p0, undefined4 p1, undefined4 p2, void *p3);
/*005b7300*/ extern int FUN_005b7300(undefined4 p0, undefined4 p1, undefined4 p2, float p3);
/*008bd7b0*/ extern char *g_sound_groups;

#define VOICE_FLAG_ACTIVE (2) /* inferred: voice is already started / playing */
#define THUNDER_ARCTIC_GROUP_OFFSET (2000) /* inferred: first voice slot index of the thunder sounds is stored here (arctic) */
#define THUNDER_DEFAULT_GROUP_OFFSET (0x620) /* same for every other course */
#define COURSE_ARCTIC (0) /* course id 0 */
#define THUNDER_VOICE_COUNT (8)
#define FREQ_MIN_BITS (0x466a6000) /* 15000.0f, random_range takes the float bit patterns */
#define FREQ_MAX_BITS (0x46ea6000) /* 30000.0f */

/* course_storm_cb1_sub_471eb0 */

/*
 * Starts one thunder voice at the position pos (x, y, z): looks through the (up to) 8 consecutive
 * voice slots that begin at the course's thunder group entry and uses the first one that is not
 * playing. Only runs during a race.
 */
/* TIER2: unvalidated 2026-10-23 authority=diffemu */
void course_storm_cb1_sub_471eb0(float *pos)
{
  int remaining = THUNDER_VOICE_COUNT;
  int slot;
  char *voice;
  int child;
  double freq;

  if (get_app_state() == APP_STATE_RACE) {
    if (current_course_id() == COURSE_ARCTIC)
      slot = *(int *)(g_sound_groups + THUNDER_ARCTIC_GROUP_OFFSET);
    else
      slot = *(int *)(g_sound_groups + THUNDER_DEFAULT_GROUP_OFFSET);
    do {
      voice = g_voices[slot].voice;
      if ((*(byte *)(voice + VOICE_OFF_FLAGS) & VOICE_FLAG_ACTIVE) == 0) {
        *(float *)(voice + VOICE_OFF_POS_X) = pos[0];
        *(float *)(voice + VOICE_OFF_POS_Y) = pos[1];
        *(float *)(voice + VOICE_OFF_POS_Z) = pos[2];
        child = *(int *)(g_voices[slot].voice + VOICE_OFF_HANDLE);
        if (child != 0)
          FUN_005b72a0(child, 0, 1, pos);
        FUN_005b6a40(g_voices[slot].voice, 1);
        freq = random_range(FREQ_MIN_BITS, FREQ_MAX_BITS);
        *(float *)(g_voices[slot].voice + VOICE_OFF_FREQ) = (float)freq;
        voice = g_voices[slot].voice;
        if (*(int *)(voice + VOICE_OFF_HANDLE) != 0) {
          freq = random_range(FREQ_MIN_BITS, FREQ_MAX_BITS);
          FUN_005b7300(*(undefined4 *)(g_voices[slot].voice + VOICE_OFF_HANDLE), 4, 1, (float)freq);
        }
        voice_set_volume(slot, 1.0f);
        voice = g_voices[slot].voice;
        *(float *)(voice + VOICE_OFF_POS_X) = pos[0];
        *(float *)(voice + VOICE_OFF_POS_Y) = pos[1];
        *(float *)(voice + VOICE_OFF_POS_Z) = pos[2];
        child = *(int *)(g_voices[slot].voice + VOICE_OFF_HANDLE);
        if (child != 0)
          FUN_005b72a0(child, 0, 1, pos);
        remaining = 0;
      }
      slot++;
      remaining--;
    } while (remaining > 0);
  }
  return;
}
