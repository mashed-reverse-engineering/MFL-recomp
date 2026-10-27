#include "mfl_tier2.h"
/*004c7150*/ extern int lua_gettop();
/*004c7220*/ extern double lua_tonumber();
/*006299e0*/ extern float g_lua_current_respawn;


/* lua_Set_Current_Respawn_Time */

/* Sets the current respawn time from the Lua argument */
undefined4 lua_Set_Current_Respawn_Time(undefined4 lua_state)
{
  undefined4 arg_count;
  double respawn_time;
  
  arg_count = lua_gettop(lua_state);
  respawn_time = (double)lua_tonumber(lua_state,arg_count);
  g_lua_current_respawn = (float)respawn_time;
  return 0;
}
