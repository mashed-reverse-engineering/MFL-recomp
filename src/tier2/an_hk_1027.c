#include "mfl_tier2.h"
/*004727a0*/ extern int sfx_play_group_at();
/*0048e410*/ extern int rwp_object_find_index_by_id();
/*0048e470*/ extern int rwp_object_sync_and_reset();
/*0048e6f0*/ extern int rwp_object_get_matrix();
/*0060d894*/ extern undefined4 g_course_egypt_60d894;
/* sound_fn_4537f0 */

/* callee category sound */

/* Egypt course: reset 3 objects (ids 5000..5002) to position (200,400,200) (float bits 0x43480000/0x43c80000), then if the g_course_egypt_60d894 flag is set play sfx group 8 at the last object's matrix translation (+0x30) and clear the flag (inferred). */
void sound_fn_4537f0(void)

{
  undefined4 object_index;
  int loop_i;
  struct {
    undefined4 pos_x;
    undefined4 pos_y;
    undefined4 pos_z;
  } frame;
  
  frame.pos_x = 0x43480000;
  frame.pos_y = 0x43c80000;
  frame.pos_z = 0x43480000;
  loop_i = 0;
  do {
    object_index = rwp_object_find_index_by_id(loop_i + 5000);
    rwp_object_sync_and_reset(object_index,&frame.pos_x);
    loop_i = loop_i + 1;
  } while (loop_i < 3);
  if (g_course_egypt_60d894 != 0) {
    loop_i = rwp_object_get_matrix(object_index);
    sfx_play_group_at(8,loop_i + 0x30);
    g_course_egypt_60d894 = 0;
  }
  return;
}


