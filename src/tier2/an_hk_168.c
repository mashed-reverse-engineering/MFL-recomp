#include "mfl_tier2.h"
/* FUN_005c43a0
 * Initialises a 5-dword record: dword 0 and dword 2 are cleared, dword 1 gets
 * type_id (inferred), dwords 3 and 4 get value_3 and value_4 (inferred).
 * Parameters: entry = record to initialise, type_id / value_3 / value_4 = initial values.
 * Returns nothing.
 */

void FUN_005c43a0(undefined4 *entry,undefined4 type_id,undefined4 value_3,undefined4 value_4)

{
  entry[1] = type_id;
  *entry = 0;
  entry[2] = 0;
  entry[3] = value_3;
  entry[4] = value_4;
  return;
}


