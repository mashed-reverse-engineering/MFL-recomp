#include "mfl_tier2.h"
/*0044c180*/ extern float __cdecl player_distance_get(int p0);
/*005e64a4*/ extern float k_f_0;
/*004131d0*/ extern int game_mode_end_check();
/*0044c180*/ extern float __cdecl player_distance_get(int p0);
/*0044d1f0*/ extern undefined ai_grid_cell_lookup_0044d1f0(undefined4 p0);
/*0044d2a0*/ extern int ai_nearest_route_point();
/*0047b520*/ extern int car_current_matrix();
/*0047b590*/ extern int car_get_forward();
/*004b2b48*/ extern int _ftol();
/*004d54d0*/ extern RwReal RwV2dLength(struct RwV2d *p0);
/*005dd330*/ extern float float_1_0;
/*005dd628*/ extern float k_f_0_25;
/*005e64a4*/ extern float k_f_0;
/*007f0284*/ extern struct AiData g_ai_data;
/*008c9980*/ extern float g_ai_skill;
/*008c9ec0*/ extern undefined4 g_ai_player_counters;
/* ai_player0_distance_is_zero */

/* "player_distance_get(0) == 0" */

undefined4 ai_player0_distance_is_zero(void)

{
  if (player_distance_get(0) == k_f_0) {
    return 1;
  }
  return 0;
}
