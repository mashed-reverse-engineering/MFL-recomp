#include "mfl_tier2.h"
/*004c7150*/ extern int lua_gettop();
/*004c7220*/ extern double lua_tonumber();
/*006f2af8*/ extern struct CourseLap *g_course_lap;


/* lua_Split_Sector */

/* Lua callback to record a split sector value for a given sector index: Split_Sector(index, value). */

undefined4 lua_Split_Sector(undefined4 lua_state)
{
  int sector_index;
  int sector_value;
  
  /* Read sector value from top of Lua stack (argument 2) */
  sector_index = lua_gettop(lua_state);
  sector_value = (int)lua_tonumber(lua_state,sector_index);

  /* Read sector index from previous stack slot (argument 1) */
  sector_index = (int)lua_tonumber(lua_state,sector_index + -1);

  /* Store split sector value in course lap data */
  g_course_lap->split_sectors[sector_index] = sector_value;
  return 0;
}
