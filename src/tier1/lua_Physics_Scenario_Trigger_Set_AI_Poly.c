#include "mfl_tier2.h"
/*004c7150*/ extern int __cdecl lua_gettop(void *L);
/*004c7220*/ extern double __cdecl lua_tonumber(void *L, int index);
/*0089df68*/ extern undefined4 g_gfx_89df68;
#define AI_POLY_ENTRY_BYTES (0x45c)
#define AI_POLY_BLOCK_BYTES (0x2510)
/* lua_Physics_Scenario_Trigger_Set_AI_Poly */
/* Lua function Physics_Scenario_Trigger_Set_AI_Poly (registered by the script engine) */
undefined4 lua_Physics_Scenario_Trigger_Set_AI_Poly(char *L)
{
  int arg_count;
  int index;
  undefined4 field_2_value;
  undefined4 field_0_value;
  undefined4 *entry;

  arg_count = lua_gettop((void *)L);
  index = (int)lua_tonumber((void *)L,arg_count + -2);
  field_2_value = (int)lua_tonumber((void *)L,arg_count + -1);
  field_0_value = (int)lua_tonumber((void *)L,arg_count);
  /* entries of 0x45c bytes, 8 per block of 0x2510 bytes */
  entry = (undefined4 *)((char *)&g_gfx_89df68 + (index / 8) * AI_POLY_BLOCK_BYTES + (index & 7) * AI_POLY_ENTRY_BYTES);
  entry[2] = field_2_value;
  entry[0] = field_0_value;
  return 0;
}
