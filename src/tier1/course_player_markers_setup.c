#include "mfl_tier2.h"
/*00481ea0*/ extern int player_marker_set_clump();
/*00481ee0*/ extern int player_marker_set_enabled();
/*0060d9d8*/ extern undefined4 g_course_player_60d9d8;
/*0060da38*/ extern char g_course_player_60da38[];
/* course_player_markers_setup */
/* "assigns the 8 marker clumps of the world record and enables them" */
void __cdecl course_player_markers_setup(char *course,char *param_2,char *param_3,char *param_4,char *param_5, char *param_6)
{
  int marker_index;
  undefined4 *clump_slot;
  
  marker_index = 0;
  clump_slot = &((CourseWorld *)course)->clumps[9];
  do {
    player_marker_set_clump(marker_index,*clump_slot);
    player_marker_set_enabled(marker_index,1);
    marker_index = marker_index + 1;
    clump_slot = clump_slot + 1;
  } while (marker_index < 8);
  clump_slot = &g_course_player_60d9d8;
  do {
    *clump_slot = 0;
    clump_slot = clump_slot + 3;
  } while ((LONG_PTR)clump_slot < ((LONG_PTR)&g_course_player_60da38));
  return;
}
