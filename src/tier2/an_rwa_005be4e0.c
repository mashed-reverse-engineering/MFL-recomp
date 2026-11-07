/* rwa_llist_remove_data_005be4e0 (0x005be4e0): data lists (RwaLListRemoveData): unlink and free the first entry (walking backwards from the head) whose data is DATA; LIST, or 0 when DATA is not in the list */
#include "mfl_tier2.h"

typedef struct RwaLListEntry { struct RwaLListEntry *next, *prev; void *data; } RwaLListEntry;

/*00949d20*/ extern undefined g_data_949d20;      /* free list of the list entries */
/*005bed80*/ extern void FUN_005bed80(void *freelist, void *p);      /* RwaFreeListFree */

/* TIER2: unvalidated 2026-11-06 authority=diffemu */
RwaLListEntry *rwa_llist_remove_data_005be4e0(RwaLListEntry *list, void *data)
{
  RwaLListEntry *e = list->prev;

  while (e->data != data) {      /* the head's own data word is compared first when the list is empty, as in the original */
    if (e == list)
      return 0;
    e = e->prev;
  }
  e->next->prev = e->prev;
  e->prev->next = e->next;
  FUN_005bed80(&g_data_949d20, e);
  return list;
}
