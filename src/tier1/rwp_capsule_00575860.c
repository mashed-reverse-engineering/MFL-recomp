/* compile: /G5 */
#include "mfl_tier2.h"
typedef struct RwpEdgeSlot { unsigned short prev, next, vertex, twin; } RwpEdgeSlot;
typedef struct RwpEdgePool { char pad[8]; RwpEdgeSlot *slots; unsigned short free_head; } RwpEdgePool;
/* Takes two slots from the free list of the edge pool and links them as twin half edges into the rings of the two vertices. (Rwp library) */
void rwp_capsule_00575860(RwpEdgePool *pool, unsigned short va, unsigned short vb)
{
  unsigned short h1 = pool->free_head, h2, t;
  if (h1 == 0xffff) return;
  h2 = pool->slots[h1].next;
  pool->free_head = h2;
  pool->free_head = pool->slots[h2].next;
  pool->slots[h1].vertex = vb;
  pool->slots[h1].twin = h2;
  pool->slots[h2].vertex = va;
  pool->slots[h2].twin = h1;
  t = pool->slots[va].next;
  pool->slots[h1].next = t;
  pool->slots[h1].prev = va;
  pool->slots[t].prev = h1;
  pool->slots[va].next = h1;
  t = pool->slots[vb].next;
  pool->slots[h2].next = t;
  pool->slots[h2].prev = vb;
  pool->slots[t].prev = h2;
  pool->slots[vb].next = h2;
}
