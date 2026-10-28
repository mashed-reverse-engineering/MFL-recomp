#include "mfl_tier2.h"
/*005be600*/ extern int __cdecl guid_compare(int a1, int a2);
/* Offset of the list head node inside the owner struct, in bytes (inferred). */
#define LIST_HEAD_OFFSET (4)
/* Object base relative to its list node, in int units: the node link sits 32 bytes into the object (inferred). */
#define OBJ_NODE_INDEX (-8)
/* FUN_005c4590 */
/*
 * Searches a circular intrusive list of objects for the first one that
 * matches key (inferred). Each object embeds a list node; the list is
 * anchored at list_head (a node embedded in the owner, at LIST_HEAD_OFFSET).
 * guid_compare(object, key) returns 0 on a match (inferred).
 *
 * Returns the matching object, or NULL if the walk reaches the head again.
 */

int * FUN_005c4590(char *list_head,undefined4 key)

{
  int *node;
  int cmp_result;

  /* Start at the first node after the head. */
  node = *(int **)(list_head + LIST_HEAD_OFFSET);
  cmp_result = guid_compare(node[OBJ_NODE_INDEX],key);
  /* Walk the ring, stopping when the head is reached again. */
  while( true ) {
    /* Match found: return the object that owns this node. */
    if (cmp_result == 0) {
      return node + OBJ_NODE_INDEX;
    }
    node = (int *)*node;
    if (node == (int *)(list_head + LIST_HEAD_OFFSET)) break;
    cmp_result = guid_compare(node[OBJ_NODE_INDEX],key);
  }
  return (int *)0x0;
}


