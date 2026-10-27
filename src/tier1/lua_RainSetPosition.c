#include "mfl_tier2.h"
/*004a03e0*/ extern undefined __cdecl line_rain_error(void *p0, undefined4 p1);
/*004a0540*/ extern int lua_RainSetPosition_impl();
/*004c7150*/ extern int __cdecl lua_gettop(void *L);
/*004c7220*/ extern double __cdecl lua_tonumber(void *L, int index);
/* number of arguments RainSetPosition takes: xmin, xmax, ymin, ymax, zmin, zmax */
#define RAIN_SET_POSITION_ARGS (6)
/* lua_RainSetPosition */
/* "'Usage : RainSetPosition(xmin, xmax, ymin ...)' Lua function" */
/* Lua binding for RainSetPosition(xmin, xmax, ymin, ymax, zmin, zmax).
   lua_state is the Lua state; the six numbers are read from the top of its stack
   (xmin is the first argument, zmax the last). Calls lua_RainSetPosition_impl with the
   raw IEEE bits of each float. Always returns 0 (no Lua results). Raises a usage
   error when the argument count is wrong (inferred). */
undefined4 lua_RainSetPosition(char *lua_state)
{
  float xmin;
  float xmax;
  float ymin;
  float ymax;
  float zmin;
  float zmax;
  int arg_count;

  arg_count = lua_gettop(lua_state);
  xmin = (float)lua_tonumber(lua_state,arg_count + -5);
  xmax = (float)lua_tonumber(lua_state,arg_count + -4);
  ymin = (float)lua_tonumber(lua_state,arg_count + -3);
  ymax = (float)lua_tonumber(lua_state,arg_count + -2);
  zmin = (float)lua_tonumber(lua_state,arg_count + -1);
  zmax = (float)lua_tonumber(lua_state,arg_count);
  if (arg_count != RAIN_SET_POSITION_ARGS) {
    line_rain_error("Usage : RainSetPosition(xmin, xmax, ymin, ymax, zmin, zmax)\n",0xffffffff);
  }
  lua_RainSetPosition_impl(*(int *)&xmin,*(int *)&xmax,*(int *)&ymin,*(int *)&ymax,*(int *)&zmin,*(int *)&zmax);
  return 0;
}
