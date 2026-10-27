#include "mfl_tier2.h"
#define NODE_NEXT_OFFSET (4)
#define NODE_VALUE_OFFSET (8)
/* FUN_004b2670
 * Pops the first node off a singly linked list (inferred).
 * list_head: pointer to the list's head pointer; advanced to the next node.
 * Returns the popped node's value, or 0 if the list is empty.
 * Node layout (inferred): next pointer at NODE_NEXT_OFFSET, value at NODE_VALUE_OFFSET.
 */
undefined4 __stdcall FUN_004b2670(int *list_head)
{
  char *node;

  /* empty list: nothing to pop */
  node = *list_head;
  if (node == 0) {
    return 0;
  }
  /* unlink the head node, then return its value */
  *list_head = *(int *)(node + NODE_NEXT_OFFSET);
  return *(undefined4 *)(node + NODE_VALUE_OFFSET);
}
