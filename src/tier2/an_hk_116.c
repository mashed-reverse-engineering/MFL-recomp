#include "mfl_tier2.h"

#define TABLE_ENTRY_SIZE (8)

/* FUN_005b72a0
 * Looks up entry `entry_index` in the table at context[1]->+4 (8-byte entries) and calls its
 * callback (inferred) with (context[0], the entry's 16-bit argument at +4, callback_arg).
 * Returns context if the callback returned nonzero, otherwise NULL (inferred).
 * param_3 is unused. */

uint FUN_005b72a0(undefined4 *context,int entry_index,char *param_3,undefined4 callback_arg)

{
  undefined4 *entry;
  int result;

  entry = (undefined4 *)(*(char **)(context[1] + 4) + entry_index * TABLE_ENTRY_SIZE);
  result = (*(int (*)())*entry)(*context,*(undefined2 *)(entry + 1),callback_arg);
  return -(uint)(result != 0) & (uint)context;
}


