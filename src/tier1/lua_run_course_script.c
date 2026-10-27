#include "mfl_tier2.h"
/*004492b0*/ extern int lua_register_course_functions();
/*0048cf80*/ extern int lua_run_script_file();
/*006299dc*/ extern undefined4 g_lua_current_type;
/*006f2adc*/ extern float g_lua_current_pos;
/*006f2ae0*/ extern float g_gfx_6f2ae0;
/*006f2ae4*/ extern float g_gfx_6f2ae4;


/* lua_run_course_script */

#define LUA_CURRENT_TYPE_NONE 0xffffffff /* "no current type" sentinel */

/* "resets the lua current pos/type and runs the script with lua_register_course_functions" */

/* Resets the Lua current position/type state, then runs the course script. */

void __cdecl lua_run_course_script(undefined4 script_path)
{
  undefined1 unused_out [4]; /* out-parameter of lua_run_script_file, value not used */

  g_lua_current_pos = 0;
  g_gfx_6f2ae0 = 0;
  g_gfx_6f2ae4 = 0;
  g_lua_current_type = LUA_CURRENT_TYPE_NONE;
  lua_run_script_file(script_path,unused_out,lua_register_course_functions);
  return;
}
