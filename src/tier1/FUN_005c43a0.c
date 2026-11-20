#include "mfl_tier2.h"
/* _RwaThreadCreate: initialises a RwaThread record (dword 0 handle and dword 2 arg cleared, dword 1 the work function,
   dword 3 the priority, dword 4 the stack size) and returns it. */
/* FUN_005c43a0
 * Parameters: entry = record to initialise, type_id = work function, value_3 = priority, value_4 = stack size.
 * Returns entry: every caller tests it (`_RwaThreadCreate(...) && _RwaThreadStart(...)`, rwa stream register, async queue,
 * rwa_unit23fad0). As `void` the result was whatever EAX held: cl 13 left `entry` there, v143 /O2 left value_4 (the stack
 * size, 0 for the stream update thread), so the v143 exe never started that thread, closed the stream list semaphore and
 * later waited on the recycled handle (race loading hang, agents/hang_w16/notes.md).
 */
undefined4 *FUN_005c43a0(undefined4 *entry,undefined4 type_id,undefined4 value_3,undefined4 value_4)
{
  entry[1] = type_id;
  *entry = 0;
  entry[2] = 0;
  entry[3] = value_3;
  entry[4] = value_4;
  return entry;
}
