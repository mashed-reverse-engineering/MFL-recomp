#include "mfl_tier2.h"
/*00481ea0*/ extern int player_marker_set_clump();
/*00481ee0*/ extern int player_marker_set_enabled();
/*0060d9d8*/ extern undefined4 g_course_player_60d9d8;
/*0060da38*/ extern char g_course_player_60da38[];
/* course_player_markers_setup */
/* "assigns the 8 marker clumps of the world record and enables them" */
void __cdecl course_player_markers_setup(char *param_1,char *param_2,char *param_3,char *param_4,char *param_5, char *param_6)
{
  int iVar1;
  undefined4 *puVar2;
  
  iVar1 = 0;
  puVar2 = (undefined4 *)(param_1 + 0xc134);
  do {
    player_marker_set_clump(iVar1,*puVar2);
    player_marker_set_enabled(iVar1,1);
    iVar1 = iVar1 + 1;
    puVar2 = puVar2 + 1;
  } while (iVar1 < 8);
  puVar2 = &g_course_player_60d9d8;
  do {
    *puVar2 = 0;
    puVar2 = puVar2 + 3;
  } while ((LONG_PTR)puVar2 < ((LONG_PTR)&g_course_player_60da38));
  return;
}
