#include "mfl_tier2.h"
/*004c7150*/ extern int lua_gettop();
/*004c7220*/ extern double lua_tonumber();
/*006f2af8*/ extern struct CourseLap *g_course_lap;
/*006f2afc*/ extern undefined4 g_lap_line_count;


/* lua_Lap_Line */

/* Lua function Lap_Line (registered by the script engine) */

undefined4 lua_Lap_Line(undefined4 param_1)

{
  undefined4 arg_count;
  int iVar1;
  
  int idx;
  arg_count = lua_gettop(param_1);
  iVar1 = (int)lua_tonumber(param_1,arg_count);
  idx = g_lap_line_count;
  g_course_lap->lap_lines[idx] = iVar1;
  g_lap_line_count = idx + 1;
  return 0;
}
