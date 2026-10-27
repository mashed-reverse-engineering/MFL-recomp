#include "mfl_tier2.h"
/*007d2128*/ extern struct RwGlobalsRelease *RwEngineInstance;


/* pool_list_destroy */

/* "frees every node of a singly linked pool list and the header" */

/* List header layout (dwords): [0] first node, [1] cursor used while freeing, [2] node count.
   Each node's first dword is the next node. Nodes and the header are freed through
   RwEngineInstance->field_0x10c (free routine, name unknown). */
undefined4 pool_list_destroy(undefined4 *pool_list)
{
  undefined4 *node;
  int freed_count;

  pool_list[1] = *pool_list;
  freed_count = 0;
  if (0 < (int)pool_list[2]) {
    do {
      node = (undefined4 *)pool_list[1];
      pool_list[1] = *node;
      RwFree(node);
      freed_count = freed_count + 1;
    } while (freed_count < (int)pool_list[2]);
  }
  RwFree(pool_list);
  return 0;
}
