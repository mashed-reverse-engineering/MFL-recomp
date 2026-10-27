#include "mfl_tier2.h"
/*005be600*/ extern int __cdecl FUN_005be600(void *a1, int a2);
#define OWNER_LIST_OFFSET (0x118)     /* owner -> list header pointer */
#define LIST_COUNT_OFFSET (0xc)       /* list header -> entry count */
#define LIST_ENTRIES_OFFSET (0x10)    /* list header -> entry array pointer */
#define LIST_ENTRY_STRIDE (0x20)      /* bytes per entry */
#define NOT_FOUND (0xffffffff)        /* return value when no entry accepted */
/*
 * FUN_005c2180: walks the entry list stored in owner (owner + OWNER_LIST_OFFSET),
 * calling FUN_005be600(entry, context) on each non-null entry in order (inferred).
 * Stops at the first entry where the callee returns 0 and returns that entry's index.
 * Returns NOT_FOUND if no entry stops the walk or the list is empty.
 *   owner:   object holding the list header pointer at OWNER_LIST_OFFSET (inferred)
 *   context: second argument forwarded to every callee call (inferred)
 */
uint FUN_005c2180(char *owner,undefined4 context)
{
  char *cur;               /* holds the list header, then each entry, then the callee result (reused) */
  uint index;
  char *entry_offset;

  cur = *(char **)(owner + OWNER_LIST_OFFSET);
  index = 0;
  /* walk the entries while index < entry count; the header is re-read each pass */
  if (0 < *(uint *)(cur + LIST_COUNT_OFFSET)) {
    entry_offset = 0;
    do {
      /* load entry at index; skip null slots, otherwise stop when the callee returns 0 */
      cur = *(char **)(entry_offset + *(int *)(cur + LIST_ENTRIES_OFFSET));
      if (cur != 0) {
        cur = FUN_005be600(cur,context);
        if (cur == 0) {
          return index;
        }
      }
      cur = *(char **)(owner + OWNER_LIST_OFFSET);
      index = index + 1;
      entry_offset = entry_offset + LIST_ENTRY_STRIDE;
    } while (index < *(uint *)(cur + LIST_COUNT_OFFSET));
  }
  return NOT_FOUND;
}
