#include "mfl_tier2.h"
/*005c21e0*/ extern int __cdecl FUN_005c21e0(void *a1, int a2);
/* Byte size of one table entry (inferred: index * size gives the entry's byte offset). */
#define ENTRY_SIZE (0x20)
/* FUN_005c30c0
 * Looks up the entry index for (obj, arg) via FUN_005c21e0, then stores into obj+0x140 the
 * byte offset of that entry in the table at *(obj+0x118)+0x10 (inferred).
 * obj: base object pointer (inferred). arg: lookup key passed through to FUN_005c21e0 (inferred).
 * Returns nothing; the result is written to obj+0x140. */
void __cdecl FUN_005c30c0(char *obj,undefined4 arg)
{
  int entry_index;

  /* Resolve which table entry this (obj, arg) pair refers to. */
  entry_index = FUN_005c21e0(obj,arg);
  *(int *)(obj + 0x140) = entry_index * ENTRY_SIZE + *(int *)(*(char **)(obj + 0x118) + 0x10);
  return;
}
