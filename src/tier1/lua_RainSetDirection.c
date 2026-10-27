#include "mfl_tier2.h"
/*004a03e0*/ extern undefined __cdecl line_rain_error(void *p0, undefined4 p1);
/*004a05c0*/ extern int lua_RainSetDirection_impl();
/*004c7150*/ extern int __cdecl lua_gettop(void *L);
/*004c7220*/ extern double __cdecl lua_tonumber(void *L, int index);
#define RAIN_DIRECTION_ARG_COUNT (6) /* xmin, xmax, ymin, ymax, zmin, zmax */
/* lua_RainSetDirection */
/* "'Usage : RainSetDirection(xmin, xmax, ymin ...)' Lua function" */
/* Lua binding for RainSetDirection(xmin, xmax, ymin, ymax, zmin, zmax).
   lua_state: the Lua state the call was made from (inferred).
   Reads the six numeric arguments, reports a usage error if the count is wrong
   (inferred: line_rain_error raises the Lua error, so execution continues after it),
   then passes each value to lua_RainSetDirection_impl as raw float bits.
   Returns 0 (no Lua results). */
undefined4 lua_RainSetDirection(char *lua_state)
{
  float xmin_f;
  float xmax_f;
  float ymin_f;
  float ymax_f;
  float zmin_f;
  float zmax_f;
  int arg_count;

  arg_count = lua_gettop(lua_state);
  xmin_f = (float)lua_tonumber(lua_state,arg_count + -5);
  xmax_f = (float)lua_tonumber(lua_state,arg_count + -4);
  ymin_f = (float)lua_tonumber(lua_state,arg_count + -3);
  ymax_f = (float)lua_tonumber(lua_state,arg_count + -2);
  zmin_f = (float)lua_tonumber(lua_state,arg_count + -1);
  zmax_f = (float)lua_tonumber(lua_state,arg_count);
  if (arg_count != RAIN_DIRECTION_ARG_COUNT) {
    line_rain_error("Usage : RainSetDirection(xmin, xmax, ymin, ymax, zmin, zmax)\n",0xffffffff);
  }
  lua_RainSetDirection_impl(*(int *)&xmin_f,*(int *)&xmax_f,*(int *)&ymin_f,*(int *)&ymax_f,*(int *)&zmin_f,*(int *)&zmax_f);
  return 0;
}
