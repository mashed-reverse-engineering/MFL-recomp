#include "mfl_tier2.h"
#define CLUMP_EXCLUDED 1

/*004c7150*/ extern int lua_gettop();
/*004c7220*/ extern double lua_tonumber();
/*006f2aec*/ extern struct CourseCounts *g_course_counts;


/* lua_Clump_Exclude_From_World */

/* Lua function Clump_Exclude_From_World (registered by the script engine) */
/* Marks a course clump as excluded from the world based on Lua argument. */

undefined4 lua_Clump_Exclude_From_World(void *lua_state)
{
  int arg_count;
  int clump_index;
  
  /* Read clump index argument from the top of the Lua stack */
  arg_count = lua_gettop(lua_state);
  clump_index = (int)lua_tonumber(lua_state,arg_count);
  /* Mark the clump as excluded from the world in course counts */
  g_course_counts->clump_exclude_from_world[clump_index] = CLUMP_EXCLUDED;
  return 0;
}
