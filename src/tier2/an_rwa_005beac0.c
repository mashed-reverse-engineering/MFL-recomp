/* rwa_heap_free_005beac0 (0x005beac0): RWA heaps (RwaHeapFree): free P into the heap block that holds it; a block that becomes empty is freed, unless it is the last one (then it is kept at the list head) */
#include "mfl_tier2.h"

typedef struct RwaHeapChunk {
  void *end;      /* end of the free span */
} RwaHeapChunk;

typedef struct RwaHeapBlock {      /* see _RwaHeapAddBlock (5be9b0) */
  struct RwaHeapBlock *next, *prev;
  RwaHeapChunk *firstFree;      /* +8 */
  char *end;                    /* +0xc: block end; the block is empty when its first free chunk reaches it */
} RwaHeapBlock;

typedef struct RwaHeap {
  RwUInt32 w00;
  RwUInt32 blockSize;           /* +4 */
  RwUInt32 hint;                /* +8 */
  RwaHeapBlock blocks;          /* +0xc: list head (next, prev) */
} RwaHeap;

/*005be970*/ extern void RwaHeapBlockFree(RwaHeapBlock *block, void *p);
/*005bf120*/ extern void FUN_005bf120(void *block);      /* RWA free */

/* TIER2: unvalidated 2026-11-10 authority=static */
void rwa_heap_free_005beac0(RwaHeap *heap, char *p)
{
  RwaHeapBlock *head = &heap->blocks, *b;

  for (b = head->next; b != head; b = b->next)
    if (p > (char *)b && p < (char *)b + heap->blockSize)
      break;
  if (b == head)
    return;
  RwaHeapBlockFree(b, p);
  if (b->firstFree->end != b->end)
    return;
  b->prev->next = b->next;
  b->next->prev = b->prev;
  if (head->next == head) {      /* that was the only block: keep it */
    b->prev = head;
    b->next = head->next;
    head->next->prev = b;
    head->next = b;
  } else
    FUN_005bf120(b);
}
