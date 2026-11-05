/* rwa_heap_create_005beb70 (0x005beb70): RWA heaps (RwaHeapCreate): a heap of item blocks, linked into the global heap list */
#include "mfl_tier2.h"

/*005bec50*/ extern void *rwa_unit243670_005bec50(void *list, RwUInt32 hint);      /* free list: allocate */
/*005bf0d0*/ extern void *FUN_005bf0d0(RwUInt32 size, RwUInt32 hint);      /* RWA malloc */
/*005bea00*/ extern void RwaHeapDestroy(void *heap);

typedef struct RwaLink { struct RwaLink *next, *prev; } RwaLink;

typedef struct RwaHeapChunk {      /* free chunk header at the start of a block's data */
  void *end;
  void *w4;
  void *w8;
} RwaHeapChunk;

typedef struct RwaHeapBlock {
  RwaLink link;            /* +0x00 in the heap's block list */
  RwaHeapChunk *free;      /* +0x08 */
  char *end;               /* +0x0c */
  /* +0x10 data */
} RwaHeapBlock;

typedef struct RwaHeap {
  RwUInt32 external;       /* +0x00 1: the caller's memory, 0: from the heap free list */
  RwUInt32 blockSize;      /* +0x04 */
  RwUInt32 extra;          /* +0x08 */
  RwaLink blocks;          /* +0x0c */
  RwaLink link;            /* +0x14 in the global heap list */
} RwaHeap;

/*007dbb98*/ extern undefined g_data_7dbb98;      /* free list of heaps */
/*007dbb90*/ extern RwaLink *g_data_7dbb90;       /* global heap list head */

#define RWA_HINT 0x30800

/* itemSize / count: items per block (each with a 0xc byte header); extra: bytes added to each block; blocks: blocks
   allocated now; mem: memory for the heap header (else from the free list).  Returns the heap or NULL. */
/* TIER2: unvalidated 2026-11-05 authority=diffemu */
RwaHeap *rwa_heap_create_005beb70(RwUInt32 itemSize, RwUInt32 count, RwUInt32 extra, RwUInt32 blocks, RwaHeap *mem)
{
  RwaHeap *heap = mem;

  if (!heap) {
    heap = (RwaHeap *)rwa_unit243670_005bec50(&g_data_7dbb98, RWA_HINT);
    if (!heap)
      return 0;
    heap->external = 0;
  } else
    heap->external = 1;
  heap->extra = extra;
  heap->blockSize = (itemSize + 0xc) * count + extra + 0x1c;
  heap->blocks.next = &heap->blocks;
  heap->blocks.prev = &heap->blocks;
  heap->link.next = g_data_7dbb90;
  heap->link.prev = (RwaLink *)&g_data_7dbb90;
  g_data_7dbb90->prev = &heap->link;
  g_data_7dbb90 = &heap->link;

  while (blocks) {
    RwUInt32 size = heap->blockSize;
    RwaHeapBlock *b;

    blocks--;
    b = (RwaHeapBlock *)FUN_005bf0d0(size, RWA_HINT);
    if (!b) {
      RwaHeapDestroy(heap);
      return 0;
    }
    b->free = (RwaHeapChunk *)(b + 1);
    b->end = (char *)b + size;
    b->free->w8 = 0;
    b->free->w4 = 0;
    b->free->end = b->end;
    b->link.next = heap->blocks.next;
    b->link.prev = &heap->blocks;
    heap->blocks.next->prev = &b->link;
    heap->blocks.next = &b->link;
  }
  return heap;
}
