/* splitBlock (MFL.exe 0x004f3210): RenderWare Graphics 3.7 (Criterion Software), splits a resource heap block, the first part taking
 * size bytes; the rest is a new unused block merged with a following unused one.
 * Adapted from codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32, src/rw/src/plcore/resmem.c @e578672,
 * not byte-identical, changes: RWFUNCTION / RWASSERT / RWRETURNVOID expanded (release build), not static (the exe calls it by
 * address from rwResHeapAlloc), the structs declared here with compile-time layout asserts.  Verified by tools/diffemu (tier 2). */
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
RW_LAYOUT_ASSERT(rw_assert_blk_next, RW_OFFSETOF(rwResHeapBlockHeader, next) == 4);
RW_LAYOUT_ASSERT(rw_assert_blk_prev, RW_OFFSETOF(rwResHeapBlockHeader, prev) == 8);
RW_LAYOUT_ASSERT(rw_assert_blk_size, RW_OFFSETOF(rwResHeapBlockHeader, size) == 0xc);
RW_LAYOUT_ASSERT(rw_assert_blk_flags, RW_OFFSETOF(rwResHeapBlockHeader, flags) == 0x10);
RW_LAYOUT_ASSERT(rw_assert_blk_sizeof, sizeof(rwResHeapBlockHeader) == 0x20);

/* TIER2: unvalidated 2026-11-09 authority=diffemu */
void splitBlock(rwResHeapBlockHeader *block, RwUInt32 size)
{
    rwResHeapBlockHeader *newBlock;

    newBlock = (rwResHeapBlockHeader *)(((RwUInt8 *)block) + size + sizeof(rwResHeapBlockHeader));

    /* Is next block free? */
    if (block->next && (~block->next->flags & rwRESHEAP_BLOCKUSED))
    {
        newBlock->next = block->next->next;
        newBlock->size = (block->size - size) + block->next->size;
    }
    else
    {
        newBlock->next = block->next;
        newBlock->size = block->size - size - sizeof(rwResHeapBlockHeader);
    }

    block->next = newBlock;
    newBlock->flags = 0;

    newBlock->prev = block;
    if (newBlock->next)
    {
        newBlock->next->prev = newBlock;
    }

    block->size = size;
    newBlock->heap = block->heap;
}
