#include "mfl_tier2.h"
/*0045c7c0*/ extern int course_atomics_flags_clear();
/*0045cff0*/ extern int anim_stop();
/*0045db10*/ extern undefined __cdecl course_player_markers_setup(undefined4 p0, undefined4 p1, undefined4 p2, undefined4 p3, undefined4 p4, void *p5);
/*00492d10*/ extern int rwp_object_set_active();
/*0060d9c8*/ extern undefined4 g_course_60d9c8;
/*0060d9cc*/ extern undefined4 g_course_60d9cc;
/*006a2888*/ extern undefined g_course_6a2888;
/* course_warzone_cb6 */
/* Course setup step for COURSE_WARZONE (inferred from the callees).
 * Stops the animation at g_course_6a2888, deactivates the two RenderWare objects
 * g_course_60d9c8 (object_a) and g_course_60d9cc (object_b), sets up the player
 * markers, then clears the course atomic flags. Returns nothing.
 * player_marker_arg: first argument of course_player_markers_setup (meaning unknown). */

void course_warzone_cb6(undefined4 player_marker_arg)

{
  undefined4 object_b;
  undefined4 zero_arg_p2;
  undefined4 object_a;
  undefined4 zero_arg_p4;
  undefined *anim_handle;

  anim_handle = &g_course_6a2888;
  anim_stop(((char *)&g_course_6a2888));
  zero_arg_p4 = 0;
  object_a = g_course_60d9c8;
  rwp_object_set_active(g_course_60d9c8, 0x0);
  zero_arg_p2 = 0;
  object_b = g_course_60d9cc;
  rwp_object_set_active(g_course_60d9cc, 0x0);
  course_player_markers_setup(player_marker_arg,object_b,zero_arg_p2,object_a,zero_arg_p4,anim_handle);
  course_atomics_flags_clear();
  return;
}


