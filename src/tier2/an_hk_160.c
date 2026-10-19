#include "mfl_tier2.h"
/* FUN_005c1eb0
   Calls one handler from a dispatch table. The table pointer sits at
   dispatch + DISPATCH_TABLE_OFFSET; each DISPATCH_ENTRY_SIZE-byte entry holds
   the handler function pointer at +0 and a 16-bit argument at +4.
   Calls handler(context, entry argument, extra_arg) for table[slot].
   context: first argument passed to the handler (inferred).
   dispatch: object holding the table pointer at +8 (inferred).
   slot: index of the entry to call.
   extra_arg: last argument passed to the handler (inferred).
   Returns nothing. */
#define DISPATCH_TABLE_OFFSET (8)
#define DISPATCH_ENTRY_SIZE (8)

void FUN_005c1eb0(undefined4 context,char *dispatch,int slot,undefined4 extra_arg)

{
  undefined4 *slot_ptr;

  slot_ptr = (undefined4 *)(*(char **)(dispatch + DISPATCH_TABLE_OFFSET) + slot * DISPATCH_ENTRY_SIZE);
  (*(code *)*slot_ptr)(context,*(undefined2 *)(slot_ptr + 1),extra_arg);
  return;
}


