#include "mfl_tier2.h"

#define NODE_LINK_OFFSET (0x14)
#define NODE_PREV_OFFSET (0x18)
#define NODE_STATE_OFFSET (0x20)
#define NODE_LIST_OFFSET (0x24)
#define LIST_HEAD_LINK_OFFSET (0xc)
#define LINK_PREV_OFFSET (4)

/* FUN_005c1dc0 */

/* Moves a list node to the front of its list and records a new state for it.
 * node: list node; its link is at NODE_LINK_OFFSET, the prev-slot pointer at
 *       NODE_PREV_OFFSET, the state object at NODE_STATE_OFFSET and the list
 *       owner at NODE_LIST_OFFSET (inferred).
 * param_2: unused.
 * new_state_ptr: points at the new state value (1 or 3 get special handling).
 * Returns nothing. */
void __cdecl FUN_005c1dc0(char *node,char *param_2,int *new_state_ptr)

{
  int *node_slot;
  int new_state;
  char *state;
  int *head_slot;

  /* Update the state object: 1 -> 8 and 3 -> 4 (inferred: transition flags at +0x28),
     unless it already holds 3 or 5 respectively. Then store the new state at +0x20. */
  new_state = *new_state_ptr;
  state = *(char **)(node + NODE_STATE_OFFSET);
  if (new_state == 1) {
    if (*(int *)(state + 0x28) != 3) {
      *(undefined4 *)(state + 0x28) = 8;
    }
  }
  else if ((new_state == 3) && (*(int *)(state + 0x28) != 5)) {
    *(undefined4 *)(state + 0x28) = 4;
  }
  *(int *)(state + 0x20) = new_state;

  /* Unlink the node if it is already in a list: its neighbours are joined
     through the prev-slot pointer and the next pointer. */
  node_slot = (int *)(node + NODE_LINK_OFFSET);
  if (*(int *)(node + NODE_LINK_OFFSET) != 0) {
    **(int **)(node + NODE_PREV_OFFSET) = *(int *)(node + NODE_LINK_OFFSET);
    *(undefined4 *)(*node_slot + LINK_PREV_OFFSET) = *(undefined4 *)(node + NODE_PREV_OFFSET);
  }
  /* Insert at the front: the node takes the list head's first link, and the head's
     slot becomes the node's prev-slot. */
  head_slot = (int *)(*(char **)(node + NODE_LIST_OFFSET) + LIST_HEAD_LINK_OFFSET);
  *node_slot = *(int *)(*(char **)(node + NODE_LIST_OFFSET) + LIST_HEAD_LINK_OFFSET);
  *(int **)(node + NODE_PREV_OFFSET) = head_slot;
  *(int **)(*head_slot + LINK_PREV_OFFSET) = node_slot;
  *(int **)(*(char **)(node + NODE_LIST_OFFSET) + LIST_HEAD_LINK_OFFSET) = node_slot;
  return;
}


