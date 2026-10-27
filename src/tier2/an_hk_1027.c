#include "mfl_tier2.h"
/*004727a0*/ extern int sfx_play_group_at();
/*0048e410*/ extern int rwp_object_find_index_by_id();
/*0048e470*/ extern int rwp_object_sync_and_reset();
/*0048e6f0*/ extern int rwp_object_get_matrix();
/*0060d894*/ extern undefined4 g_course_egypt_60d894;
/* sound_fn_4537f0 */

/* callee category sound */

void sound_fn_4537f0(void)

{
  undefined4 uVar1;
  int iVar2;
  struct {
    undefined4 uStack_c;
    undefined4 uStack_8;
    undefined4 uStack_4;
  } frame;
  
  frame.uStack_c = 0x43480000;
  frame.uStack_8 = 0x43c80000;
  frame.uStack_4 = 0x43480000;
  iVar2 = 0;
  do {
    uVar1 = rwp_object_find_index_by_id(iVar2 + 5000);
    rwp_object_sync_and_reset(uVar1,&frame.uStack_c);
    iVar2 = iVar2 + 1;
  } while (iVar2 < 3);
  if (g_course_egypt_60d894 != 0) {
    iVar2 = rwp_object_get_matrix(uVar1);
    sfx_play_group_at(8,iVar2 + 0x30);
    g_course_egypt_60d894 = 0;
  }
  return;
}


