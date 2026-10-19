#include "mfl_tier2.h"
/*007d2128*/ extern struct RwGlobalsRelease *RwEngineInstance;
/* offsets inside the name table pointed to by owner+0x118 (inferred) */
#define NAME_TABLE_PTR_OFFSET (0x118)
#define NAME_TABLE_COUNT_OFFSET (0xc)
#define NAME_TABLE_ENTRIES_OFFSET (0x10)
#define NAME_TABLE_ENTRY_SIZE (0x20)
#define NAME_ENTRY_NAME_OFFSET (4)
#define NAME_NOT_FOUND (0xffffffff)
/*
 * Finds a name in the name table of owner (table pointer at owner+0x118, inferred).
 * Compares each non-NULL entry name with `name` using the RwEngineInstance
 * vecStrcmp function (inferred: plain strcmp, 0 = match).
 *
 * owner: object holding the name table pointer (inferred).
 * name:  string to look for.
 * Returns the index of the first matching entry, or NAME_NOT_FOUND (-1) if none matches.
 * (inferred: the table pointer is reused as the strcmp result local.)
 */
uint FUN_005c21e0(char *owner,RwChar *name)

{
  RwChar *entry_name;
  char *table;
  uint index;
  int entry_offset;

  table = *(char **)(owner + NAME_TABLE_PTR_OFFSET);
  index = 0;
  if (*(int *)(table + NAME_TABLE_COUNT_OFFSET) != 0) {
    entry_offset = 0;
    /* walk the entries: each entry is NAME_TABLE_ENTRY_SIZE bytes, name pointer at +4 */
    do {
      entry_name = *(RwChar **)(*(char **)(table + NAME_TABLE_ENTRIES_OFFSET) + NAME_ENTRY_NAME_OFFSET + entry_offset);
      /* skip empty slots; a match returns the current index */
      if (entry_name != (RwChar *)0x0) {
        table = rwstrcmp(entry_name,name);
        if (table == 0) {
          return index;
        }
      }
      table = *(char **)(owner + NAME_TABLE_PTR_OFFSET);
      index = index + 1;
      entry_offset = entry_offset + NAME_TABLE_ENTRY_SIZE;
    } while (index < *(uint *)(table + NAME_TABLE_COUNT_OFFSET));
  }
  return NAME_NOT_FOUND;
}


