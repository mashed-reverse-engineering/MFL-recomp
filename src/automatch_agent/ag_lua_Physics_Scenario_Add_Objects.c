#include "mfl_tier2.h"
/*004c7150*/ extern int lua_gettop();
/*004c7220*/ extern double lua_tonumber();
/*0089de40*/ extern undefined4 g_gfx_89de40;


/* lua_Physics_Scenario_Add_Objects */

/* Lua function Physics_Scenario_Add_Objects (registered by the script engine) */

undefined4 lua_Physics_Scenario_Add_Objects(undefined4 lua_state)

{
  int arg_count;
  int scenario_index;
  undefined4 object;
  
  arg_count = lua_gettop(lua_state);
  scenario_index = (int)lua_tonumber(lua_state,arg_count + -1);
  object = (int)lua_tonumber(lua_state,arg_count);
  (&g_gfx_89de40)[scenario_index * 0x944 + (&g_gfx_89de40)[scenario_index * 0x944 + 8]] = object;
  (&g_gfx_89de40)[scenario_index * 0x944 + 8]++;
  return 0;
}
