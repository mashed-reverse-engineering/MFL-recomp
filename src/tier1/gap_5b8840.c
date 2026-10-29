#include "mfl_tier2.h"
/* gap_5b8840 */

/* no context */

/* Unlink node from its current list and insert it at the front of list_head's list, setting node->owner = list_head (inferred intrusive doubly-linked list). */
void gap_5b8840(char *node,int list_head)

{
  int *node_next_link;
  int *head_first_link;
  
  node_next_link = (int *)(node + 0x14);
  if (*(int *)(node + 0x14) != 0) {
    **(int **)(node + 0x18) = *(int *)(node + 0x14);
    *(undefined4 *)(*node_next_link + 4) = *(undefined4 *)(node + 0x18);
  }
  head_first_link = (int *)(list_head + 0xc);
  *node_next_link = *(int *)(list_head + 0xc);
  *(int **)(node + 0x18) = head_first_link;
  *(int **)(*head_first_link + 4) = node_next_link;
  *head_first_link = (int)node_next_link;
  *(int *)(node + 0x24) = list_head;
  return;
}
