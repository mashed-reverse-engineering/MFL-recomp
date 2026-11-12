/* rwa_llist_get_count_005be5a0 (0x005be5a0): RwaLListGetCount, number of entries of an RWA linked list */
#include "mfl_tier2.h"

/* The list head is a circular sentinel node: { prev, next, ... }; next is at +4.  The head itself is not counted. */
/* TIER2: unvalidated 2026-11-10 authority=static */
int rwa_llist_get_count_005be5a0(void **head)
{
  void **node;
  int count = 0;

  for (node = (void **)head[1]; node != head; node = (void **)node[1])
    count++;
  return count;
}
