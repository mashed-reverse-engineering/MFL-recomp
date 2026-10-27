#include "mfl_tier2.h"
/*004c7150*/ extern int lua_gettop();
/*004c7220*/ extern double lua_tonumber();
/*006f2aec*/ extern struct CourseCounts *g_course_counts;


/* lua_Setup_Fog */

/* Lua function Setup_Fog (registered by the script engine) */
/* Args: fog range start, fog range end (floats), then fog_color[0..2] (bytes). fog_color[3] is fixed at 0xff. Field meanings beyond that: unknown. */

undefined4 lua_Setup_Fog(undefined4 lua_state)
{
  CourseCounts *counts;
  byte byte_value;
  int arg_count;
  double float_value;

  arg_count = lua_gettop(lua_state);
  /* arg_count - 4: range start */
  float_value = (double)lua_tonumber(lua_state,arg_count + -4);
  g_course_counts->fog_range[0] = (float)float_value;
  /* arg_count - 3: range end */
  float_value = (double)lua_tonumber(lua_state,arg_count + -3);
  g_course_counts->fog_range[1] = (float)float_value;
  byte_value = (int)lua_tonumber(lua_state,arg_count + -2);
  g_course_counts->fog_color[0] = byte_value;
  byte_value = (int)lua_tonumber(lua_state,arg_count + -1);
  g_course_counts->fog_color[1] = byte_value;
  byte_value = (int)lua_tonumber(lua_state,arg_count);
  counts = g_course_counts;
  g_course_counts->fog_color[2] = byte_value;
  counts->fog_color[3] = 0xff;   /* alpha, fixed */
  return 0;
}
