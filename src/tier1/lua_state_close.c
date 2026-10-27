#include "mfl_tier2.h"
/*004c7610*/ extern void FUN_004c7610(undefined4 p0);
/*006f2b00*/ extern undefined4 g_lua_state;
/* lua_state_close */
/* "closes g_lua_state and clears it" */
/* Closes the global Lua state (g_lua_state) by passing it to FUN_004c7610 (inferred: Lua close/teardown),
 * then sets g_lua_state to 0 so later code sees no state. Takes no parameters, returns nothing. */
void lua_state_close(void)
{
  FUN_004c7610(g_lua_state);
  g_lua_state = 0;
  return;
}
