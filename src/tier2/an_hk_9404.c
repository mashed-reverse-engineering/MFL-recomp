#include "mfl_tier2.h"
extern struct VoiceSlot g_voices[210];
/*0046a0c0*/ extern int voice_set_volume(int param_2, float param_3);
/*00481fd0*/ extern double random_range();
/*005b6a40*/ extern int FUN_005b6a40(undefined4 p0, undefined4 p1);
/*005b7300*/ extern int FUN_005b7300(undefined4 p0, undefined4 p1, undefined4 p2, float p3);
/*005ddb18*/ extern float float_0_8;
/*005df99c*/ extern float k_f_22050;
/*0061794c*/ extern float g_sound_frame_61794c;
/*00619e84*/ extern undefined4 g_sound_frame_619e84;
/* sound_ambient_pitch_randomise */

/* "starts the ambience voice with random pitch (22050 - random) and volume 0.8" */

void __cdecl sound_ambient_pitch_randomise(char *unused_1,char *unused_2,char *unused_3,char *unused_4,
          char *unused_5)

{
  int iVar1;
  undefined4 pu_eax;
  double fVar2;
  
  iVar1 = g_sound_frame_619e84;
  if ((*(byte *)(g_voices[g_sound_frame_619e84].voice + 0x34) & 2) == 0) {
    FUN_005b6a40(g_voices[g_sound_frame_619e84].voice,1);
    fVar2 = (double)random_range(0,0x459c4000);
    *(float *)(g_voices[iVar1].voice + 0x3c) = (float)((double)k_f_22050 - fVar2);
    pu_eax = 0;
    if (*(int *)(g_voices[iVar1].voice + 0x30) != 0) {
      fVar2 = (double)random_range(0,0x459c4000);
      pu_eax = FUN_005b7300(*(undefined4 *)(g_voices[iVar1].voice + 0x30),4,1,
                             (float)((double)k_f_22050 - fVar2));
    }
    voice_set_volume(iVar1,g_sound_frame_61794c * float_0_8);
  }
  return;
}


