#include "mfl_tier2.h"
/*004a0580*/ extern int __cdecl lua_RainSetScale_impl(float p0, float p1);
/*004c7150*/ extern int lua_gettop();
/*004c7220*/ extern double lua_tonumber();
extern void __cdecl line_rain_error_x(void *p0, undefined4 p1);
undefined4 lua_RainSetScale(undefined4 lua_state)

{
  int arg_count;
  float scale;
  float length;
  
  arg_count = lua_gettop(lua_state);
  scale = (float)lua_tonumber(lua_state,arg_count + -1);
  length = (float)lua_tonumber(lua_state,arg_count);
  if (arg_count != 2) {
    line_rain_error_x("Usage : RainSetScale(scale, length)\n",0xffffffff);
  }
  lua_RainSetScale_impl(scale,length);
  return 0;
}
