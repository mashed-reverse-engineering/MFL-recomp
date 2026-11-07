// cluster-entries: rwa_heap_add_block_005be9b0 rwa_heap_alloc_005bea60
/* Installed cluster rwa_heap_add_block_005be9b0 (tools/cluster_install.py from output/cluster_pilot_lanea/rwa_heap_add_block_005be9b0.c); entries are patched at their original addresses by build_overlay.py --clusters. */
/* RWA heaps: _RwaHeapAddBlock (register function: heap in edi, entered through the generated thunk) allocates a block of the heap's
   block size (+4) whose whole body is one free chunk and puts it at the front of the block list (+0xc); RwaHeapAlloc (5bea60, its
   caller) tries every block, then a new one. */
#include "mfl_tier2.h"

typedef struct RwaHeapChunk {
  void *end;      /* end of the free span */
  RwUInt32 w04, w08;
} RwaHeapChunk;

typedef struct RwaHeapBlock {
  struct RwaHeapBlock *next, *prev;
  RwaHeapChunk *firstFree;      /* +8 */
  char *end;                    /* +0xc */
} RwaHeapBlock;

typedef struct RwaHeap {
  RwUInt32 w00;
  RwUInt32 blockSize;           /* +4 */
  RwUInt32 hint;                /* +8: allocation hint passed to the block allocator */
  RwaHeapBlock blocks;          /* +0xc: list head (next, prev) */
} RwaHeap;

/*005bf0d0*/ extern void *FUN_005bf0d0(RwUInt32 size, RwUInt32 hint);      /* RWA malloc */
/*005be8e0*/ extern void *RwaHeapBlockAlloc(RwaHeapBlock *block, RwUInt32 size, RwUInt32 hint);

/* TIER2: unvalidated 2026-11-06 authority=diffemu */
RwaHeapBlock *rwa_heap_add_block_005be9b0_n(RwaHeap *heap)
{
  RwUInt32 size = heap->blockSize;
  RwaHeapBlock *b = FUN_005bf0d0(size, 0x30800), *head;

  if (!b)
    return 0;
  b->firstFree = (RwaHeapChunk *)(b + 1);
  b->end = (char *)b + size;
  b->firstFree->w08 = 0;
  b->firstFree->w04 = 0;
  b->firstFree->end = b->end;
  head = &heap->blocks;
  b->next = head->next;
  b->prev = head;
  head->next->prev = b;
  head->next = b;
  return b;
}

/* TIER2: unvalidated 2026-11-06 authority=diffemu */
void *rwa_heap_alloc_005bea60(RwaHeap *heap, RwUInt32 size)
{
  RwaHeapBlock *head = &heap->blocks, *b = head->next;
  void *p = 0;

  while (b != head) {
    p = RwaHeapBlockAlloc(b, size, heap->hint);
    b = b->next;
    if (p)
      return p;
  }
  b = rwa_heap_add_block_005be9b0_n(heap);
  if (!b)
    return p;
  return RwaHeapBlockAlloc(b, size, heap->hint);
}

/* entry thunk at the original address of rwa_heap_add_block_005be9b0: register inputs (edi) -> natural C rwa_heap_add_block_005be9b0_n; ret 0 */
__declspec(naked) void rwa_heap_add_block_005be9b0(void)
{
    __asm {
        push edi
        call rwa_heap_add_block_005be9b0_n
        add esp, 4
        ret
    }
}
