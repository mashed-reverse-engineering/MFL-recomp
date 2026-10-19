#include "mfl_tier2.h"
/*004c7150*/ extern int __cdecl lua_gettop(void *L);
/*004c7220*/ extern double __cdecl lua_tonumber(void *L, int index);
/*006f2aec*/ extern struct CourseCounts *g_course_counts;
/* lua_Vehicle_Shininess_Range */

/* Lua function Vehicle_Shininess_Range (registered by the script engine)
 * Sets the course's vehicle shininess range from the last two Lua arguments
 * (range_min = second to last, range_max = last), stores them as
 * g_course_counts->vehicle_shininess_range[0..1] and returns 0 results.
 * L is the lua_State (inferred). The min/max naming is inferred from the store order. */

undefined4 lua_Vehicle_Shininess_Range(char *L)

{
  CourseCounts *counts;
  int arg_count;
  float10 range_min;
  float10 range_max;

  /* the two values are the top two stack entries (inferred from lua_gettop - 1 and lua_gettop) */
  arg_count = lua_gettop(L);
  range_min = (float10)lua_tonumber(L,arg_count + -1);
  range_max = (float10)lua_tonumber(L,arg_count);
  counts = g_course_counts;
  g_course_counts->vehicle_shininess_range[0] = (float)range_min;
  counts->vehicle_shininess_range[1] = (float)range_max;
  return 0;
}


