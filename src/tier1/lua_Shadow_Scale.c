#include "mfl_tier2.h"
/*004c7150*/ extern int lua_gettop();
/*004c7220*/ extern double lua_tonumber();
/*006f2aec*/ extern struct CourseCounts *g_course_counts;
/*004c7150*/ extern int lua_gettop();
/*004c7220*/ extern double lua_tonumber();
/*006f2aec*/ extern struct CourseCounts *g_course_counts;
/* lua_Shadow_Scale */

/* Sets the course shadow scale factor in course counts from the Lua script argument. */

undefined4 lua_Shadow_Scale(undefined4 lua_state)
{
  int arg_count;
  double shadow_scale;
  
  /* Retrieve the scale argument from the top of the Lua stack */
  arg_count = lua_gettop(lua_state);
  shadow_scale = (double)lua_tonumber(lua_state,arg_count);
  g_course_counts->shadow_scale = (float)shadow_scale;
  return 0;
}
