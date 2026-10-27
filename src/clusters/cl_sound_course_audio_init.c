// cluster-entries: sound_course_audio_init
/* Installed cluster sound_course_audio_init (tools/cluster_install.py from output/cluster_pilot_kb/sound_course_audio_init.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
extern char PTR_g_data_622e60_006290b0[];
/*006aaf94*/ extern undefined4 g_sound_load_6aaf94[0x1000];
/*006aaf9c*/ extern undefined4 g_sound_load_6aaf9c;
/*006ac6f0*/ extern undefined g_sound_load_6ac6f0;
/* sound_course_audio_init */

/* "initialises the 0xA2A2A2A2 slot table for a course" */

void sound_course_audio_init(undefined1 *header16, int course_or_range_idx)

{
  undefined *course_audio;
  undefined4 bank_id;
  undefined4 *next_slot;
  undefined4 *slot;
  int group;
  int group_off;
  int *range;
  int count;
  
  /* course_audio = per-course sound table (row param_2 of g_data_622e60): 0x28 groups of 0x30 bytes, +0x10 bank id, +0x14 range count, +0x18 pairs (first slot, slot count); slots are 10 dwords (+0 bank, +4 group+1) in g_sound_load. header16 (16 bytes) is copied into the course table and into g_sound_load. */
  course_audio = ((undefined4 *)PTR_g_data_622e60_006290b0)[course_or_range_idx];
  slot = g_sound_load_6aaf94;
  do {
    next_slot = slot + 10;
    slot[-5] = 0xa2a2a2a2;
    slot[-4] = 0xa2a2a2a2;
    slot[-3] = 0xa2a2a2a2;
    slot[-2] = 0xa2a2a2a2;
    *slot = 0;
    slot[1] = 0;
    slot[3] = 0;
    slot[2] = 0;
    slot[4] = 0;
    slot = next_slot;
  } while ((int)next_slot < (int)&g_sound_load_6ac6f0 + 0x14);
  group = (course_audio - header16);
  group_off = (int)&g_sound_load_6ac6f0 - (int)header16;
  count = 0x10;
  do {
    header16[group] = *header16;
    header16[group_off] = *header16;
    header16 = header16 + 1;
    count = count + -1;
  } while (count != 0);
  group = 0;
  do {
    group_off = group * 0x30;
    bank_id = *(undefined4 *)(course_audio + group_off + 0x10);
    course_or_range_idx = 0;
    if (0 < *(int *)(course_audio + group_off + 0x14)) {
      range = (int *)(course_audio + group_off + 0x18);
      do {
        count = range[1];
        if (0 < count) {
          slot = &g_sound_load_6aaf9c + *range * 10;
          do {
            slot[1] = group + 1;
            *slot = bank_id;
            slot = slot + 10;
            count = count + -1;
          } while (count != 0);
        }
        course_or_range_idx = course_or_range_idx + 1;
        range = range + 2;
      } while (course_or_range_idx < *(int *)(course_audio + group_off + 0x14));
    }
    group = group + 1;
  } while (group < 0x28);
  return;
}

