#include "mfl_tier2.h"
/* FUN_005c1e80 */
#define DISPATCH_TABLE_OFFSET (4)
#define DISPATCH_ENTRY_SIZE (8)

/* Calls entry `index` of the dispatch table: each 8-byte entry is a handler
   function pointer followed by a 16-bit id. The handler is called as
   handler(obj, id, arg). `dispatch` points at the table header; the table
   pointer is at offset 4. (inferred) */
void FUN_005c1e80(undefined4 obj,char *dispatch,int index,undefined4 arg)

{
  undefined4 *entry;

  entry = (undefined4 *)(*(char **)(dispatch + DISPATCH_TABLE_OFFSET) + index * DISPATCH_ENTRY_SIZE);
  (*(code *)*entry)(obj,*(undefined2 *)(entry + 1),arg);
  return;
}


