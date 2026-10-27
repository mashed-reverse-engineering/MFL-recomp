#include "mfl_tier2.h"
/*00453be0*/ extern void func_0x00453be0();
/*00459090*/ extern void __cdecl course_set_lod(int lod);
/*00459810*/ extern int course_object_offsets_init();
/*00459e50*/ extern int course_objects_reset_physics();
/* course_highway_cb6 */
/* "resets the course objects and sets level of detail 1 (tail call of course_set_lod)" */
void course_highway_cb6(undefined4 st_arg1)
{
  course_objects_reset_physics();
  course_object_offsets_init();
  func_0x00453be0();
  course_set_lod(1);
  return;
}
