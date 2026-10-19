#include "mfl_tier2.h"
/* FUN_005b7330 */

#define OWNER_TABLE_OFFSET (4)  /* offset of the entry table pointer in the owner (inferred) */
#define ENTRY_SIZE (8)          /* bytes per table entry (inferred) */

/*
 * Calls the handler stored in table entry `entry_index` of obj's owner (inferred).
 * obj[0] is passed to the handler as its first argument, the entry's 16-bit
 * tag (entry + 4) as the second, and `arg` as the third.
 * `param_3` is never used.
 * Returns obj when the handler returns non-zero, otherwise 0 (also 0 when obj is NULL).
 */
uint FUN_005b7330(undefined4 *obj,int entry_index,char *param_3,undefined4 arg)

{
  undefined4 *entry;
  int handler_result;

  if (obj == (undefined4 *)0x0) {
    return 0;
  }
  entry = (undefined4 *)(*(char **)(obj[1] + OWNER_TABLE_OFFSET) + entry_index * ENTRY_SIZE);
  handler_result = (*(int (*)())*entry)(*obj,*(undefined2 *)(entry + 1),arg);
  return -(uint)(handler_result != 0) & (uint)obj;
}
