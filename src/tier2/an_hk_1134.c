#include "mfl_tier2.h"
/*005b72a0*/ extern int sound_voice_call_entry(undefined4 p0, undefined4 p1, undefined4 p2, void *p3);

/* sound voice emitter: the voice handle at +0x30, two 3-float positions at +0x58 and +0x64 */
typedef struct VoiceEmitter {
  char unknown_00[0x30];
  int voice_handle;
  char unknown_34[0x24];
  undefined4 pos_first[3];
  undefined4 pos_second[3];
} VoiceEmitter;

/* gap_5b6b20 */

/* no context */

VoiceEmitter *gap_5b6b20(VoiceEmitter *param_1,char *unused_2,undefined4 *param_3)

{
  param_1->pos_first[0] = *param_3;
  param_1->pos_first[1] = param_3[1];
  param_1->pos_first[2] = param_3[2];
  if (param_1->voice_handle != 0) {
    sound_voice_call_entry(param_1->voice_handle,0,1,param_3);
  }
  return param_1;
}
