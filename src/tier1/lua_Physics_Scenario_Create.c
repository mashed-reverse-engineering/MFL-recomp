#include "mfl_tier2.h"
/*004c7150*/ extern int __cdecl lua_gettop(void *L);
/*004c7260*/ extern char * lua_tostring(void * p0, int p1);
/*004c72d0*/ extern void __cdecl lua_pushnumber(void *L, double n);
/*006c3d98*/ extern undefined4 g_physics_scenario_count;
/*0089de20*/ extern undefined1 g_physics_scenarios;
#define SCENARIO_SLOT_SIZE (0x2510) /* bytes reserved per scenario in g_physics_scenarios (inferred) */
/* lua_Physics_Scenario_Create */
/* Lua function Physics_Scenario_Create (registered by the script engine)
 *
 * Creates a new physics scenario slot. The scenario name is taken from the last
 * Lua argument (inferred) and copied, NUL terminator included, into the next free
 * slot of g_physics_scenarios. Bumps g_physics_scenario_count and pushes the new
 * scenario's 0-based index as a Lua number.
 *
 * lua_state: Lua state passed in by the script engine.
 * Returns: 1 (number of values pushed onto the Lua stack).
 */
undefined4 lua_Physics_Scenario_Create(char *lua_state)
{
  char ch;
  int scenario_index;
  undefined4 arg_count;
  char *src;
  char *dst;

  /* name is the last argument (inferred) */
  arg_count = lua_gettop(lua_state);
  /* new scenario gets the index equal to the current count */
  scenario_index = g_physics_scenario_count;
  src = (char *)lua_tostring(lua_state,arg_count);
  dst = &g_physics_scenarios + scenario_index * SCENARIO_SLOT_SIZE;
  /* copy the name byte by byte, up to and including the NUL terminator */
  do {
    ch = *src;
    src = src + 1;
    *dst = ch;
    dst = dst + 1;
  } while (ch != '\0');
  /* register the new scenario and return its index to Lua */
  g_physics_scenario_count = g_physics_scenario_count + 1;
  lua_pushnumber(lua_state,(double)scenario_index);
  return 1;
}
