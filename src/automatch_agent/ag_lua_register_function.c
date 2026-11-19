#include "mfl_tier2.h"
/*004c73e0*/ extern int __cdecl lua_setglobal(undefined4 p0, undefined4 p1);
/*006f2b00*/ extern undefined4 g_lua_state;


/* lua_register_function */

/* "(function, name): lua_register on the global state: the helper that every registered Lua
   function of the game goes through (found by lua_api.py)" */

void lua_register_function(undefined4 param_1,undefined4 param_2)

{
  thunk_FUN_004c8160(g_lua_state,param_2,0);
  lua_setglobal(g_lua_state,param_1);
  return;
}
