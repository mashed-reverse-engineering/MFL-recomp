#include "mfl_tier2.h"
/*005be600*/ extern int __cdecl FUN_005be600(int a1, int a2);
/*007db7ec*/ extern undefined4 g_data_7db7ec;
/* Entry sits 8 words (32 bytes) before the list node pointer (inferred) */
#define ENTRY_FROM_NODE_WORDS (-8)
/*
 * FUN_005bda80: look up an entry in the circular list headed by g_data_7db7ec.
 * key: value passed to the comparison callback FUN_005be600 (inferred).
 * Returns the first entry for which FUN_005be600(entry, key) returns 0,
 * or NULL if key is 0 or no entry matches (inferred).
 */
undefined4 * FUN_005bda80(int key)

{
  int cmp_result;
  undefined4 *node;
  undefined4 *entry;

  /* Null key never matches: bail out early */
  if (key == 0) {
    return (undefined4 *)0x0;
  }
  node = g_data_7db7ec;
  /* Empty list when the head points back to itself */
  if ((undefined4 **)g_data_7db7ec != &g_data_7db7ec) {
    /* Walk the list, comparing each entry against key */
    do {
      entry = node + ENTRY_FROM_NODE_WORDS;
      cmp_result = FUN_005be600(*entry,key);
      if (cmp_result == 0) {
        return entry;
      }
      node = (undefined4 *)*node;
    } while ((undefined4 **)node != &g_data_7db7ec);
  }
  return (undefined4 *)0x0;
}


