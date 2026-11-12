/* rwa_llist_empty_005be560 (0x005be560): data lists (RwaLListEmpty): unlink and free every entry of the list */
#include "mfl_tier2.h"

typedef struct RwaLListEntry { struct RwaLListEntry *next, *prev; void *data; } RwaLListEntry;

/*00949d20*/ extern undefined g_data_949d20;      /* free list of the list entries */
/*005bed80*/ extern void FUN_005bed80(void *freelist, void *p);      /* RwaFreeListFree */

/* TIER2: unvalidated 2026-11-10 authority=static */
void rwa_llist_empty_005be560(RwaLListEntry *list)
{
  RwaLListEntry *e;

  while ((e = list->prev) != list) {
    e->next->prev = e->prev;
    e->prev->next = e->next;
    FUN_005bed80(&g_data_949d20, e);
  }
}
