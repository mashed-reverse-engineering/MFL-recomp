#include "mfl_tier2.h"
/*005b8c70*/ extern void FUN_005b8c70(undefined4 p0, void *p1);
/*006ae07c*/ extern undefined4 g_sound_stream_handles;
/* voice_set_two_params */
/*
 * Sets two parameters on a voice's sound stream by calling FUN_005b8c70 twice with the same
 * 4-word argument block (inferred).
 *   voice:         index into g_sound_stream_handles (inferred)
 *   value:         first word of the block
 *   first_param:   word 3 of the block, used by the first call
 *   second_param:  word 3 of the block, used by the second call
 * Block layout: {value, 0, flag, param}; flag is 0 for the first call and 1 for the second.
 */
void __cdecl voice_set_two_params(int voice,undefined4 value,undefined4 first_param,undefined4 second_param)
{
  struct {
    undefined4 local_10;
    undefined4 local_c;
    undefined4 local_8;
    undefined4 local_4;
  } args;

  args.local_10 = value;
  args.local_c = 0;
  args.local_8 = 0;
  args.local_4 = first_param;
  FUN_005b8c70((&g_sound_stream_handles)[voice],&args.local_10);
  args.local_4 = second_param;
  args.local_8 = 1;
  FUN_005b8c70((&g_sound_stream_handles)[voice],&args.local_10);
  return;
}
