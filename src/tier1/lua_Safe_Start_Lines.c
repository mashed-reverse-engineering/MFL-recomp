#include "mfl_tier2.h"
/*004c7150*/ extern int lua_gettop();
/*004c7220*/ extern double lua_tonumber();
/*006f2af8*/ extern struct CourseLap *g_course_lap;


/* Lua Safe_Start_Lines(a, b): stores the last two arguments as 16-bit values in the
   lap line table, at the slot given by CourseLap.safe_start_count, then increments that count.
   Registered by the script engine. */
undefined4 lua_Safe_Start_Lines(undefined4 L)
{
  int slot;
  CourseLap *lap;
  undefined2 value;
  int arg_count;

  arg_count = lua_gettop(L);
  slot = g_course_lap->safe_start_count;
  /* second-to-last argument goes to the table at lap_lines + 0x800 (16-bit) */
  value = (int)lua_tonumber(L,arg_count + -1);
  *(undefined2 *)(g_course_lap->lap_lines + slot + 0x200) = value;
  /* last argument goes to lap_lines + 0x802 + slot*4 (16-bit) */
  value = (int)lua_tonumber(L,arg_count);
  lap = g_course_lap;
  *(undefined2 *)((int)g_course_lap->lap_lines + slot * 4 + 0x802) = value;
  lap->safe_start_count = lap->safe_start_count + 1;
  return 0;
}
