#include "mfl_tier2.h"
/*0048ce30*/ extern int lua_init();
/*0048ce50*/ extern int lua_state_close();
/*0048cea0*/ extern int __cdecl lua_run_piz_script(undefined4 p0, undefined4 p1);


/* lua_run_script_file */

/* Initializes Lua, executes a callback, runs a script file, and closes the Lua state. */

undefined4 lua_run_script_file(undefined4 script_file,undefined4 unknown,code *register_callback)
{
  undefined4 script_result;
  
  lua_init();
  /* Invoke callback (e.g. to register custom Lua functions) */
  (*register_callback)();
  /* Run the script file (purpose of second argument is unknown) */
  script_result = lua_run_piz_script(script_file,unknown);
  lua_state_close();
  return script_result;
}
