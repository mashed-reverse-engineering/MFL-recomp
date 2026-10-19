#include "mfl_tier2.h"
/* FUN_005c1ff0
 * Sends one 4-byte value to each of two objects, through the handler each object
 * reaches via obj->+0x28 (inferred: a per-object dispatch table).
 * obj_a gets value_a, obj_b gets value_b. Both calls use the same payload slot.
 * obj_a's handler is read at +8, obj_b's at +4 (inferred: different message slots).
 * Returns nothing.
 */

void __cdecl FUN_005c1ff0(int obj_b,undefined4 value_b,int obj_a,undefined4 value_a)

{
  char *handler;
  undefined4 payload [2];

  payload[0] = value_a;
  handler = *(char **)(*(char **)(obj_a + 0x28) + 8);
  (**(code **)(handler + 8))(obj_a,*(undefined2 *)(handler + 0xc),payload);
  payload[0] = value_b;
  handler = *(char **)(*(char **)(obj_b + 0x28) + 4);
  (**(code **)(handler + 8))(obj_b,*(undefined2 *)(handler + 0xc),payload);
  return;
}


