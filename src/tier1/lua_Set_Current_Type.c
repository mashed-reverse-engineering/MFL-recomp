#include "mfl_tier2.h"
/*004c7150*/ extern int lua_gettop();
/*004c7220*/ extern double lua_tonumber();
/*006299dc*/ extern undefined4 g_lua_current_type;


/* lua_Set_Current_Type */

/* Sets g_lua_current_type from the numeric argument at the top of the Lua stack. */
undefined4 lua_Set_Current_Type(void *lua_state)
{
  int arg_count;
  
  /* Retrieve the top argument index and convert to integer */
  arg_count = lua_gettop(lua_state);
  g_lua_current_type = (int)lua_tonumber(lua_state,arg_count);
  return 0;
}
