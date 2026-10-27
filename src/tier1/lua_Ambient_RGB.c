#include "mfl_tier2.h"
/*004c7150*/ extern int lua_gettop();
/*004c7220*/ extern double lua_tonumber();
/*006f2aec*/ extern struct CourseCounts *g_course_counts;

enum RgbaChannel {
  RGBA_RED = 0,
  RGBA_GREEN = 1,
  RGBA_BLUE = 2,
  RGBA_ALPHA = 3
};

#define LUA_ARG_OFFSET_RED   -2
#define LUA_ARG_OFFSET_GREEN -1
#define OPAQUE_ALPHA         1.0
#define LUA_NUM_RESULTS      0


/* lua_Ambient_RGB */

/* Sets course ambient RGBA lighting from Lua script arguments (r, g, b) with opaque alpha. */
/* Lua function Ambient_RGB (registered by the script engine) */

undefined4 lua_Ambient_RGB(undefined4 lua_state)
{
  CourseCounts *course_counts;
  int arg_count;
  double color_value;
  
  /* Query argument count from Lua stack */
  arg_count = lua_gettop(lua_state);
  /* Read (r, g, b) arguments from stack and store into ambient RGBA */
  color_value = (double)lua_tonumber(lua_state,arg_count + LUA_ARG_OFFSET_RED);
  g_course_counts->ambient_rgba[RGBA_RED] = (float)color_value;
  color_value = (double)lua_tonumber(lua_state,arg_count + LUA_ARG_OFFSET_GREEN);
  g_course_counts->ambient_rgba[RGBA_GREEN] = (float)color_value;
  color_value = (double)lua_tonumber(lua_state,arg_count);
  /* Store blue channel and set alpha to fully opaque */
  course_counts = g_course_counts;
  g_course_counts->ambient_rgba[RGBA_BLUE] = (float)color_value;
  course_counts->ambient_rgba[RGBA_ALPHA] = OPAQUE_ALPHA;
  return LUA_NUM_RESULTS;
}
