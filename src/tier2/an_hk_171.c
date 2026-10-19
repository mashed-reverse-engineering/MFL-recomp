#include "mfl_tier2.h"
/* FUN_005c4610 */

#define LIST_NODE_OFFSET_WORDS (-8)

/*
 * Walks a circular linked list and returns the element at position `index`.
 * list: list head; list[0] = element count, list[1] = sentinel node (inferred).
 * Each node's first word is the pointer to the next node (inferred).
 * The node is embedded 32 bytes into its element, so the element is node - 8 ints.
 * Returns NULL if index is past the count or the list is empty (inferred).
 */
int * FUN_005c4610(int *list,int index)

{
  int *node;
  int step;

  if ((index <= *list) && (*list != 0)) {
    node = (int *)list[1];
    step = 0;
    /* follow the next pointer until we are on the wanted node,
       stopping with NULL if we wrap back to the sentinel */
    if (0 < index) {
      do {
        node = (int *)*node;
        if (node == list + 1) {
          return (int *)0x0;
        }
        step = step + 1;
      } while (step < index);
    }
    return node + LIST_NODE_OFFSET_WORDS;
  }
  return (int *)0x0;
}


