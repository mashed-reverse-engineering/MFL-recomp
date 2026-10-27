#include "mfl_tier2.h"
/*004c7150*/ extern int lua_gettop();
/*004c7220*/ extern double lua_tonumber();
/*006f2aec*/ extern struct CourseCounts *g_course_counts;


/* lua_Modify_Fog */

/* Lua function Modify_Fog (registered by the script engine) */

/* Reads the last 6 Lua arguments into the course's modified fog values (fog_modify_b, fog_modify_a[0..1],
   fog_color_modified[0..2]); the alpha byte fog_color_modified[3] is always set to 0xff. Returns 0 results. */
undefined4 lua_Modify_Fog(undefined4 L)
{
  CourseCounts *counts;
  byte color_byte;
  int arg_count;
  double value;
  
  arg_count = lua_gettop(L);
  /* the 6 arguments are the top 6 stack slots: arg_count-5 .. arg_count */
  value = (double)lua_tonumber(L,arg_count + -5);
  g_course_counts->fog_modify_b = (float)value;
  value = (double)lua_tonumber(L,arg_count + -4);
  g_course_counts->fog_modify_a[0] = (float)value;
  value = (double)lua_tonumber(L,arg_count + -3);
  g_course_counts->fog_modify_a[1] = (float)value;
  color_byte = (int)lua_tonumber(L,arg_count + -2);
  g_course_counts->fog_color_modified[0] = color_byte;
  color_byte = (int)lua_tonumber(L,arg_count + -1);
  g_course_counts->fog_color_modified[1] = color_byte;
  color_byte = (int)lua_tonumber(L,arg_count);
  counts = g_course_counts;
  g_course_counts->fog_color_modified[2] = color_byte;
  counts->fog_color_modified[3] = 0xff;
  return 0;
}
