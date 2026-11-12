/* rwFreeListFreeReal (MFL.exe 0x004e9460): RenderWare Graphics 3.7 (Criterion Software), marks a free list entry unused and
 * releases its block when the block became empty.
 * Adapted from codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32, src/rw/src/plcore/bamemory.c @e578672,
 * not byte-identical, changes: RWFUNCTION / RWASSERT / RWRETURN / RWMESSAGE expanded (release build), RwFree expanded to the
 * engine's memoryFuncs slot, the rwLinkList macros and FreeListBlockIsEmpty inlined (as the exe has them), the world arena test
 * of codfh (dword_50F250 / dword_50F254, a port patch) is not in the exe and is left out, the structs declared here with
 * compile-time layout asserts.  Verified by tools/diffemu (tier 2). */
#include "mfl_tier2.h"

#define RW_OFFSETOF(type, member) ((char *)&((type *)0)->member - (char *)0)      /* pointer difference: no pointer-to-int cast */
#define RW_LAYOUT_ASSERT(name, cond) typedef char name[(sizeof(void *) != 4 || (cond)) ? 1 : -1]      /* cl 13.00 has no _Static_assert; the offsets are the 32-bit exe's */

typedef struct RwLLLink {
    struct RwLLLink *next;
    struct RwLLLink *prev;
} RwLLLink;
typedef struct RwFreeList {
    RwUInt32 entrySize;
    RwUInt32 entriesPerBlock;
    RwUInt32 heapSize;
    RwUInt32 alignment;
    RwLLLink blockList;
    RwUInt32 flags;
    RwLLLink link;
} RwFreeList;
RW_LAYOUT_ASSERT(rw_assert_link_size, sizeof(RwLLLink) == 8);
RW_LAYOUT_ASSERT(rw_assert_fl_entries, RW_OFFSETOF(RwFreeList, entriesPerBlock) == 4);
RW_LAYOUT_ASSERT(rw_assert_fl_heap, RW_OFFSETOF(RwFreeList, heapSize) == 8);
RW_LAYOUT_ASSERT(rw_assert_fl_blocks, RW_OFFSETOF(RwFreeList, blockList) == 0x10);
RW_LAYOUT_ASSERT(rw_assert_fl_flags, RW_OFFSETOF(RwFreeList, flags) == 0x18);
RW_LAYOUT_ASSERT(rw_assert_fl_size, sizeof(RwFreeList) == 0x24);

#define rwFREELISTFLAG_FREEBLOCKS 0x2

/*007d2128*/ extern void *RwEngineInstance;

/* TIER2: validated 2026-11-10 authority=diffemu */
RwFreeList *_rwFreeListFreeReal(RwFreeList *freeList, void *entry)
{
    RwUInt32 heapEntries;
    RwUInt8 *dataBlock;
    RwLLLink *link, *lastLink;

    heapEntries = freeList->heapSize;

    link = freeList->blockList.next;
    lastLink = &freeList->blockList;

    while (link != lastLink)
    {
        dataBlock = (RwUInt8 *)link + sizeof(RwLLLink) + heapEntries;

        if ((RwUInt8 *)entry >= dataBlock &&
            (RwUInt8 *)entry <= dataBlock + (freeList->entriesPerBlock * freeList->entrySize))
        {
            RwUInt8 *heap;
            RwUInt32 entryIndex;
            RwUInt32 heapElement;
            RwUInt8 mask;

            entryIndex = (RwUInt32)((RwUInt8 *)entry - dataBlock) / freeList->entrySize;

            heapElement = entryIndex >> 3;
            mask = (RwUInt8)(0x80 >> (entryIndex - (heapElement << 3)));

            heap = (RwUInt8 *)link + sizeof(RwLLLink);

            heap[heapElement] &= (RwUInt8)(~mask);

            if (freeList->flags & rwFREELISTFLAG_FREEBLOCKS)
            {
                /* FreeListBlockIsEmpty(heap, heapEntries) */
                RwUInt32 k;
                RwUInt32 sum = 0;

                for (k = 0; k < heapEntries; ++k)
                {
                    sum += heap[k];
                }
                if (sum == 0)
                {
                    link->prev->next = link->next;
                    link->next->prev = link->prev;
                    RwFree(link);
                }
            }
            return freeList;
        }
        link = link->next;
    }

    return NULL;
}
