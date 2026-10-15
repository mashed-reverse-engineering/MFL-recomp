/* functions of batch15.c that use a name declared with a different type in another source (tools/reorg.py); the preamble of batch15.c is kept as it was */
#pragma auto_inline(off)
#include "mfl_consts.h"
#include "mfl_types.h"
extern int g_effect_pool_6c5e14[], g_effect_pool_6c5e1c[];
extern int g_rw_6fcdd8[], g_rw_6fa4b0[], g_rw_6fd0f8[], g_rw_6fa858[], g_rw_6fab78[];
extern float g_rw_6fa190[];
extern struct CourseLap *g_course_lap;
extern int g_lap_line_count;
extern int g_lua_state;
extern int lua_open(int);
extern void lua_mathlib_open(int);
extern int g_lua_current_pos[], g_lua_current_type, g_lua_current_respawn;
extern int powerup_place(int *, int);
extern void powerup_set_respawn_time(int, int);
/* prototypes of functions that live in other modules */


// FUNCTION: MFL 0x0048c6e0
int lua_Place_Powerup(void)
{
    int idx = powerup_place(g_lua_current_pos, g_lua_current_type);
    powerup_set_respawn_time(idx, g_lua_current_respawn);
    return 0;
}
