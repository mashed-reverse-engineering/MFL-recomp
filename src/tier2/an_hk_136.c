#include "mfl_tier2.h"

#define HANDLER_TABLE_PTR_OFFSET (8)
#define HANDLER_ENTRY_SIZE (8)

/* FUN_005b72d0 */

/* Calls entry `entry_index` of the handler table (inferred) and passes `arg` to it.
 *   handler_set[0]  : first argument forwarded to the handler (inferred)
 *   handler_set[1]  : pointer to the table holder; the table pointer is at +HANDLER_TABLE_PTR_OFFSET
 *   param_3         : unused
 * Entries are HANDLER_ENTRY_SIZE bytes: handler function pointer, then a 16-bit value at +4.
 * Returns handler_set if the handler returned nonzero, otherwise 0 (inferred). */
uint FUN_005b72d0(undefined4 *handler_set,int entry_index,char *param_3,undefined4 arg)

{
  undefined4 *entry;
  int result;

  entry = (undefined4 *)(*(char **)(handler_set[1] + HANDLER_TABLE_PTR_OFFSET) + entry_index * HANDLER_ENTRY_SIZE);
  /* call the handler with its context, its 16-bit value and arg */
  result = (*(int (*)())*entry)(*handler_set,*(undefined2 *)(entry + 1),arg);
  /* success: return handler_set; failure: return 0 */
  return -(uint)(result != 0) & (uint)handler_set;
}


