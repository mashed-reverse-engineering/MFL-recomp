#include "mfl_tier2.h"
/*004c7c00*/ extern int FUN_004c7c00(undefined4 p0, undefined4 p1, undefined4 p2, undefined4 p3);
/*006f2b00*/ extern undefined4 g_lua_state;
/* lua_run_buffer_ok */

/* "runs FUN_004c7c00 on g_lua_state, stores the result code, returns result == 0" */

/*
 * Runs a Lua buffer on the global Lua state (inferred: FUN_004c7c00 takes the state,
 * the buffer, its size and a trailing 0 flag).
 *   buffer      - Lua source/bytecode to run (inferred)
 *   buffer_size - length of buffer in bytes (inferred)
 *   result_code - optional out pointer (may be NULL); receives the raw status returned by FUN_004c7c00
 * Returns true when the status is 0 (success).
 */
int lua_run_buffer_ok(undefined4 buffer,undefined4 buffer_size,int *result_code)

{
  int status;

  status = FUN_004c7c00(g_lua_state,buffer,buffer_size,0);
  if (result_code != (int *)0x0) {
    *result_code = status;
  }
  return status == 0;
}


