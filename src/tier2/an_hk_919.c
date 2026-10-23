#include "mfl_tier2.h"
typedef int CourseId;
extern struct VoiceSlot g_voices[210];
/*0040fdc0*/ extern int race_get_phase();
/*0042b110*/ extern int current_course_id();
/*00469b20*/ extern int sound_music_fade_step();
/*00469f40*/ extern int sound_master_fade_towards(float param_2);
/*005b6a40*/ extern int FUN_005b6a40(undefined4 p0, undefined4 p1);
/*005ddc28*/ extern float k_f_0;
/*00617948*/ extern float g_sound_master_fade;
/*008bd7b0*/ extern char *g_sound_groups;
/* sound_music_state_update */
#define COURSE_DEMR (18)
#define COURSE_DEM (24)

/* "race phase 7: master fade to 0, music fade; course dependent music state" */

undefined4 sound_music_state_update(void)

{
  int iVar1;
  CourseId CVar2;
  
  iVar1 = race_get_phase();
  if (iVar1 == 7) {
    sound_master_fade_towards(0.0);
    sound_music_fade_step(1);
    if (g_sound_master_fade <= k_f_0) {
      CVar2 = current_course_id();
      if (((CVar2 == COURSE_DEM) || (CVar2 == COURSE_DEMR)) &&
         ((*(byte *)(g_voices[*(int *)(g_sound_groups + 0x620)].voice + 0x34) & 2) != 0)) {
        FUN_005b6a40(g_voices[*(int *)(g_sound_groups + 0x620)].voice,0);
      }
    }
    return 1;
  }
  return 0;
}





