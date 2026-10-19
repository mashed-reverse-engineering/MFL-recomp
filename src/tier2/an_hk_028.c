#include "mfl_tier2.h"
/*004c7150*/ extern int __cdecl lua_gettop(void *L);
/*004c7220*/ extern double __cdecl lua_tonumber(void *L, int index);
/*004c72d0*/ extern void __cdecl lua_pushnumber(void *L, double n);
/*0089de20*/ extern undefined1 g_physics_scenarios;
/*0089df68*/ extern undefined4 g_gfx_89df68;
/*008a0248*/ extern undefined4 g_gfx_8a0248;

#define SCENARIO_SLOT_STRIDE (0x944)
#define TRIGGER_RECORD_STRIDE (0x117)
#define TRIGGER_FIELD_TYPE (0xc)
#define TRIGGER_FIELD_ENABLED (0xd)
#define TRIGGER_RECORD_LAST_WORD (0x116)
#define TRIGGER_HANDLE_SLOT_SCALE (8)

/* lua_Physics_Scenario_Create_Trigger */

/* Lua function Physics_Scenario_Create_Trigger (registered by the script engine).
 *
 * Appends a new trigger to a scenario's trigger list and returns a handle to it.
 * Lua arguments (inferred): (scenario_slot, trigger_type, start_disabled); the last
 * one is the top of the stack. The trigger is stored in the global trigger table
 * g_gfx_89df68, one record of TRIGGER_RECORD_STRIDE words per trigger, and the
 * per-slot trigger count lives in g_gfx_8a0248.
 * Returns 1 (one Lua result): the handle arg_count + scenario_slot * 8 (inferred).
 */

undefined4 lua_Physics_Scenario_Create_Trigger(void *L)

{
  int arg_count;
  int scenario_slot;
  undefined4 trigger_type;
  int start_disabled;
  char *slot;
  char *rec;

  arg_count = lua_gettop(L);
  scenario_slot = (int)lua_tonumber(L,arg_count + -2);
  trigger_type = (int)lua_tonumber(L,arg_count + -1);
  start_disabled = (int)lua_tonumber(L,arg_count);
  slot = (char *)&g_physics_scenarios + scenario_slot * SCENARIO_SLOT_STRIDE * 4;
  arg_count = *(int *)(slot + 0x2428);
  rec = slot + 0x148 + arg_count * 0x45c;
  *(undefined4 *)(rec + 0x30) = trigger_type;
  *(int *)(rec + 0x34) = 1 - start_disabled;
  *(undefined4 *)(rec + 0x458) = 0;
  *(int *)(slot + 0x2428) = *(int *)(slot + 0x2428) + 1;
  lua_pushnumber(L,(double)(arg_count + scenario_slot * TRIGGER_HANDLE_SLOT_SCALE));
  return 1;
}
