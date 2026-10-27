#include "mfl_tier2.h"
/*004a03e0*/ extern undefined __cdecl line_rain_error(void *p0, undefined4 p1);
/*004a0510*/ extern undefined __cdecl ui_colour_b_set(undefined4 p0, undefined4 p1, undefined4 p2, undefined4 p3);
/*004c7150*/ extern int lua_gettop();
/*004c7220*/ extern double lua_tonumber();
/*0076e3c0*/ extern undefined4 g_app_76e3c0;


/* lua_RainSetTailColour */

/* Lua function RainSetTailColour (registered by the script engine) */

undefined4 lua_RainSetTailColour(undefined4 param_1)
{
  int arg_count;
  undefined4 uVar1;
  undefined4 uVar2;
  undefined4 uVar3;
  undefined4 uVar4;
  
  arg_count = lua_gettop(param_1);
  uVar1 = (int)lua_tonumber(param_1,arg_count + -3);
  uVar2 = (int)lua_tonumber(param_1,arg_count + -2);
  uVar3 = (int)lua_tonumber(param_1,arg_count + -1);
  uVar4 = (int)lua_tonumber(param_1,arg_count);
  if (arg_count != 4) {
    line_rain_error("Usage : RainSetTailColour(red,green,blue,alpha)\n",0xffffffff);
  }
  if (g_app_76e3c0 == 0) {
    line_rain_error("Line Rain has not been enabled\n",0xffffffff);
  }
  ui_colour_b_set(uVar1,uVar2,uVar3,uVar4);
  return 0;
}
