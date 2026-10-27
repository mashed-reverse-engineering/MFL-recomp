#include "mfl_tier2.h"
/*004c7150*/ extern int lua_gettop();
/*004c7220*/ extern double lua_tonumber();
/*006f2aec*/ extern struct CourseCounts *g_course_counts;


/* lua_Shine_Scale */

// Lua binding: Sets the shine scale factor in global course counts from the top Lua argument
int lua_Shine_Scale(void *lua_state)
{
  int top_index;
  double shine_scale_val;
  
  top_index = lua_gettop(lua_state);
  shine_scale_val = (double)lua_tonumber(lua_state,top_index);
  g_course_counts->shine_scale = (float)shine_scale_val;
  return 0;
}
