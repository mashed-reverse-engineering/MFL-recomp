#include "mfl_tier2.h"
/*004c7150*/ extern int lua_gettop();
/*004c7220*/ extern double lua_tonumber();
/*006f2adc*/ extern float g_lua_current_pos;
/*006f2ae0*/ extern float g_gfx_6f2ae0;
/*006f2ae4*/ extern float g_gfx_6f2ae4;


#define POS_X_STACK_OFFSET -2
#define POS_Y_STACK_OFFSET -1
#define LUA_NO_RESULTS 0


/* lua_Set_Current_Position */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* Lua function Set_Current_Position (registered by the script engine) */
/* Purpose: Sets the current 3D position (x, y, z) in global state from Lua script arguments. */

undefined4 lua_Set_Current_Position(undefined4 lua_state)
{
  int arg_count;
  double coord_val;
  
  arg_count = lua_gettop(lua_state);
  /* Extract x, y, z coordinates relative to top of stack and store into position globals */
  coord_val = (double)lua_tonumber(lua_state,arg_count + POS_X_STACK_OFFSET);
  g_lua_current_pos = (float)coord_val;
  coord_val = (double)lua_tonumber(lua_state,arg_count + POS_Y_STACK_OFFSET);
  g_gfx_6f2ae0 = (float)coord_val;
  coord_val = (double)lua_tonumber(lua_state,arg_count);
  g_gfx_6f2ae4 = (float)coord_val;
  return LUA_NO_RESULTS;
}
