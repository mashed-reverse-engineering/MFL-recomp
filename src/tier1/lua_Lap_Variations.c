#include "mfl_tier2.h"
/*004c7150*/ extern int lua_gettop();
/*004c7220*/ extern double lua_tonumber();
/*006f2af8*/ extern struct CourseLap *g_course_lap;


/* lua_Lap_Variations */

/* Sets the number of course lap variations from the Lua script argument. */
/* Lua function Lap_Variations (registered by the script engine) */

undefined4 lua_Lap_Variations(undefined4 lua_state)
{
  int arg_count;
  int lap_variations;
  
  arg_count = lua_gettop(lua_state);
  lap_variations = (int)lua_tonumber(lua_state,arg_count);
  g_course_lap->lap_variations = lap_variations;
  return 0;
}
