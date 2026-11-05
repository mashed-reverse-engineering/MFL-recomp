/* rwa_llist_add_data_005be4a0 (0x005be4a0): RwaLListAddData, append a data pointer to an RWA linked list */
#include "mfl_tier2.h"

/*005bec50*/ extern void *FUN_005bec50(void *freelist, int hint);      /* RwaFreeListAlloc */
/*00949d20*/ extern undefined g_data_949d20;      /* free list of the list entries */

/* Entry = { prev, next, data }; the head is a circular sentinel, new entries go in front of it (= at the tail). */
typedef struct RwaLLink {
  struct RwaLLink *prev;
  struct RwaLLink *next;
  void *data;
} RwaLLink;

/* TIER2: unvalidated 2026-11-05 authority=diffemu */
RwaLLink *rwa_llist_add_data_005be4a0(RwaLLink *head, void *data)
{
  RwaLLink *e = (RwaLLink *)FUN_005bec50(&g_data_949d20, 0x30804);

  if (e == 0)
    return 0;
  e->data = data;
  e->prev = head->prev;
  e->next = head;
  head->prev->next = e;
  head->prev = e;
  return e;
}
