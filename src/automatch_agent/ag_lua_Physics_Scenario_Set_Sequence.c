#include "mfl_tier2.h"
/*00480130*/ extern int text_page_reset_line();
/*00480150*/ extern int text_page_next();
/*00480190*/ extern int text_page_add_string_id();
/*004c7150*/ extern int lua_gettop();
/*004c7220*/ extern double lua_tonumber();


/* lua_Physics_Scenario_Set_Sequence */

/* Lua function Physics_Scenario_Set_Sequence (registered by the script engine) */

undefined4 lua_Physics_Scenario_Set_Sequence(undefined4 param_1)

{
  int i;
  int arg_count;
  undefined4 uVar2;
  undefined4 auStack_100 [64];
  
  i = 0;
  arg_count = lua_gettop(param_1);
  text_page_reset_line();
  for (; i < arg_count; i++) {
    uVar2 = (int)lua_tonumber(param_1,i + 1);
    auStack_100[i] = uVar2;
    text_page_add_string_id(uVar2);
  }
  text_page_next();
  return 0;
}
