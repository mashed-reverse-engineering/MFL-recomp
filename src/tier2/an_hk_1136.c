#include "mfl_tier2.h"
/*005b72a0*/ extern int sound_voice_call_entry(undefined4 p0, undefined4 p1, undefined4 p2, void *p3);
/* gap_5b6b80 */

/* no context */

int gap_5b6b80(int param_1,char *unused_2,undefined4 *param_3)

{
  *(undefined4 *)(param_1 + 100) = *param_3;
  *(undefined4 *)(param_1 + 0x68) = param_3[1];
  *(undefined4 *)(param_1 + 0x6c) = param_3[2];
  if (*(int *)(param_1 + 0x30) != 0) {
    sound_voice_call_entry(*(int *)(param_1 + 0x30),1,1,param_3);
  }
  return param_1;
}


