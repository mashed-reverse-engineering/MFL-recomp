#include "mfl_tier2.h"
/*00481ec0*/ extern char *player_marker_get_clump();
/*00481ee0*/ extern int player_marker_set_enabled();
/*004866d0*/ extern int fx_emitter_spawn_b_default();
/*00499b40*/ extern int explosion_decal();
/*004b2bbd*/ extern int __cdecl FID_conflict__wprintf(char *p0, ...);
/*008bf060*/ extern undefined4 g_course_warzone_8bf060;
/* course_warzone_cb1_sub_45da60 */

/* single caller course_warzone_cb1_sub_45db70 */

/* Triggers an explosion effect (fx emitter, decal, sub effect) at the player marker's world position (frame+0x40), then disables the marker; returns the position pointer (inferred). */
float * course_warzone_cb1_sub_45da60(int player_index)

{
  float *pos;
  char *marker_frame;
  struct {
    undefined4 up_x;
    undefined4 up_y;
    undefined4 up_z;
  } frame;
  
  marker_frame = player_marker_get_clump(player_index);
  marker_frame = *(char **)(marker_frame + 4);
  frame.up_x = 0;
  frame.up_y = 0x3f800000;
  frame.up_z = 0;
  pos = (float *)(marker_frame + 0x40);
  FID_conflict__wprintf
            ("trigger explosion at (%.3f, %.3f, %.3f\n",(double)*pos,
             (double)*(float *)(marker_frame + 0x44),(double)*(float *)(marker_frame + 0x48));
  course_effect_spawn_at_helper_45c8f0(pos,0x41f00000);
  fx_emitter_spawn_b_default(pos,&frame.up_x,0x42340000,0x40000000);
  course_warzone_cb1_sub_45da00((&g_course_warzone_8bf060)[player_index],marker_frame + 0x10);
  explosion_decal(pos,0x40000000,0x3ff33333);
  player_marker_set_enabled(player_index,0);
  return pos;
}


