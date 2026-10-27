#include "mfl_tier2.h"
/*004a03e0*/ extern undefined __cdecl line_rain_error(void *p0, undefined4 p1);
/*004a0510*/ extern undefined __cdecl ui_colour_b_set(undefined4 p0, undefined4 p1, undefined4 p2, undefined4 p3);
/*004c7150*/ extern int lua_gettop();
/*004c7220*/ extern double lua_tonumber();
/*0076e3c0*/ extern undefined4 g_app_76e3c0;


/* lua_RainSetTailColour */

/* Lua function RainSetTailColour (registered by the script engine) */

undefined4 lua_RainSetTailColour(undefined4 L)
{
  int arg_count;
  undefined4 red;
  undefined4 green;
  undefined4 blue;
  undefined4 alpha;
  
  arg_count = lua_gettop(L);
  red = (int)lua_tonumber(L,arg_count + -3);
  green = (int)lua_tonumber(L,arg_count + -2);
  blue = (int)lua_tonumber(L,arg_count + -1);
  alpha = (int)lua_tonumber(L,arg_count);
  if (arg_count != 4) {
    line_rain_error("Usage : RainSetTailColour(red,green,blue,alpha)\n",0xffffffff);
  }
  if (g_app_76e3c0 == 0) {
    line_rain_error("Line Rain has not been enabled\n",0xffffffff);
  }
  ui_colour_b_set(red,green,blue,alpha);
  return 0;
}
