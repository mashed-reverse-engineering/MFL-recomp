/* compile: /G5 */
#include "mfl_tier2.h"
typedef struct RwpEdgeSlot { unsigned short prev, next, vertex, twin; } RwpEdgeSlot;
typedef struct RwpEdgePool { char pad[8]; RwpEdgeSlot *slots; unsigned short free_head; } RwpEdgePool;
/* Unlinks every half edge of a vertex ring together with its twin and returns both slots to the free list. (Rwp library) */
void rwp_capsule_00575950(RwpEdgePool *pool, unsigned short v)
{
  unsigned short cur, twin, a, b;
  cur = pool->slots[v].next;
  while (cur != v) {
    a = pool->slots[cur].next;
    twin = pool->slots[cur].twin;
    b = pool->slots[cur].prev;
    pool->slots[a].prev = b;
    pool->slots[b].next = a;
    a = pool->slots[twin].next;
    b = pool->slots[twin].prev;
    pool->slots[a].prev = b;
    pool->slots[b].next = a;
    pool->slots[cur].next = pool->free_head;
    pool->slots[twin].next = cur;
    pool->free_head = twin;
    cur = pool->slots[v].next;
  }
}
