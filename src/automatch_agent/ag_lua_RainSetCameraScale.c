#include "mfl_tier2.h"
/*004a05a0*/ extern int __cdecl lua_RainSetCameraScale_impl(float p0, float p1);
/*004c7150*/ extern int lua_gettop();
/*004c7220*/ extern double lua_tonumber();
extern void __cdecl line_rain_error_x(void *p0, undefined4 p1);
undefined4 lua_RainSetCameraScale(undefined4 lua_state)

{
  int arg_count;
  float y_scale;
  float at_scale;
  
  arg_count = lua_gettop(lua_state);
  y_scale = (float)lua_tonumber(lua_state,arg_count + -1);
  at_scale = (float)lua_tonumber(lua_state,arg_count);
  if (arg_count != 2) {
    line_rain_error_x("Usage : RainSetCameraScale(yScale, atScale)\n",0xffffffff);
  }
  lua_RainSetCameraScale_impl(y_scale,at_scale);
  return 0;
}
