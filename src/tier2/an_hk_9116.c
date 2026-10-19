#include "mfl_tier2.h"
/*004b2b48*/ extern int _ftol();
/*004c7150*/ extern int __cdecl lua_gettop(void *L);
/*004c7220*/ extern double __cdecl lua_tonumber(void *L, int index);
/*0089df68*/ extern undefined4 g_gfx_89df68;
/* lua_Physics_Scenario_Trigger_Set_AI_Poly */

/* Lua function Physics_Scenario_Trigger_Set_AI_Poly (registered by the script engine) */

undefined4 lua_Physics_Scenario_Trigger_Set_AI_Poly(undefined4 param_1)

{
  int arg_count;
  int index;
  undefined4 uVar2;
  undefined4 uVar3;
  undefined4 *entry;

  arg_count = lua_gettop((void *)param_1);
  index = (int)lua_tonumber((void *)param_1,arg_count + -2);
  uVar2 = (int)lua_tonumber((void *)param_1,arg_count + -1);
  uVar3 = (int)lua_tonumber((void *)param_1,arg_count);
  /* entries of 0x45c bytes, 8 per block of 0x2510 bytes */
  entry = (undefined4 *)((char *)&g_gfx_89df68 + (index / 8) * 0x2510 + (index & 7) * 0x45c);
  entry[2] = uVar2;
  entry[0] = uVar3;
  return 0;
}
