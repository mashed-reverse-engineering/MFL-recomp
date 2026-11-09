/* rwResHeapAlloc (MFL.exe 0x004f3190): RenderWare Graphics 3.7 (Criterion Software), allocates size bytes (32 byte rounded) from a
 * resource heap, first fit from the first free block, or NULL.
 * Adapted from codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32, src/rw/src/plcore/resmem.c @e578672,
 * not byte-identical, changes: RWFUNCTION / RWASSERT / RWRETURN expanded (release build), the dead `#if 0` smallest-fit test
 * dropped, the structs declared here with compile-time layout asserts.  Verified by tools/diffemu (tier 2). */
#include "mfl_tier2.h"

#define RW_OFFSETOF(type, member) ((char *)&((type *)0)->member - (char *)0)      /* pointer difference: no pointer-to-int cast */
#define RW_LAYOUT_ASSERT(name, cond) typedef char name[(sizeof(void *) != 4 || (cond)) ? 1 : -1]      /* cl 13.00 has no _Static_assert; the offsets are the 32-bit exe's */

#define rwRESHEAP_BLOCKUSED 1

typedef struct rwResHeapBlockHeader rwResHeapBlockHeader;
struct rwResHeapBlockHeader {
    void *heap;
    rwResHeapBlockHeader *next;
    rwResHeapBlockHeader *prev;
    RwUInt32 size;
    RwUInt32 flags;
    RwUInt32 pad[3];
};
typedef struct rwResHeapHeader {
    rwResHeapBlockHeader *firstBlock;
    rwResHeapBlockHeader *firstFreeBlock;
} rwResHeapHeader;
RW_LAYOUT_ASSERT(rw_assert_blk_next, RW_OFFSETOF(rwResHeapBlockHeader, next) == 4);
RW_LAYOUT_ASSERT(rw_assert_blk_size, RW_OFFSETOF(rwResHeapBlockHeader, size) == 0xc);
RW_LAYOUT_ASSERT(rw_assert_blk_flags, RW_OFFSETOF(rwResHeapBlockHeader, flags) == 0x10);
RW_LAYOUT_ASSERT(rw_assert_blk_sizeof, sizeof(rwResHeapBlockHeader) == 0x20);
RW_LAYOUT_ASSERT(rw_assert_heap_free, RW_OFFSETOF(rwResHeapHeader, firstFreeBlock) == 4);

/*004f3210*/ extern void splitBlock(rwResHeapBlockHeader *block, RwUInt32 size);

/* TIER2: unvalidated 2026-11-09 authority=diffemu */
void *_rwResHeapAlloc(void *resHeap, RwUInt32 size)
{
    rwResHeapHeader *heapInfo = (rwResHeapHeader *)resHeap;
    rwResHeapBlockHeader *targetBlock, *curBlock;

    /* Round allocation size up to next 32 byte multiple */
    size += 31;
    size &= 0xFFFFFFE0;

    targetBlock = (rwResHeapBlockHeader *)NULL;
    curBlock = heapInfo->firstFreeBlock;
    while (curBlock && !targetBlock)
    {
        if ((~curBlock->flags & rwRESHEAP_BLOCKUSED) && curBlock->size >= size)
        {
            targetBlock = curBlock;
        }
        curBlock = curBlock->next;
    }

    if (!targetBlock)
    {
        return NULL;
    }

    /* Split off excess into new block (if it's worth it) */
    if (targetBlock->size > (size + 64))
    {
        splitBlock(targetBlock, size);
    }

    /* May need to find the new first free block */
    if (targetBlock == heapInfo->firstFreeBlock)
    {
        do
        {
            heapInfo->firstFreeBlock = heapInfo->firstFreeBlock->next;
        }
        while (heapInfo->firstFreeBlock && (heapInfo->firstFreeBlock->flags & rwRESHEAP_BLOCKUSED));
    }

    targetBlock->flags = rwRESHEAP_BLOCKUSED;

    return (void *)(targetBlock + 1);
}
