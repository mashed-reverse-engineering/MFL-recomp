/* rwFreeListAllocReal (MFL.exe 0x004e9340): RenderWare Graphics 3.7 (Criterion Software), takes an unused entry from a free list,
 * adding a block when all are full.
 * Adapted from codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32, src/rw/src/plcore/bamemory.c @e578672,
 * not byte-identical, changes: RWFUNCTION / RWASSERT / RWRETURN expanded (release build), RwMalloc expanded to the engine's
 * memoryFuncs slot, the rwLinkList macros inlined, the alignment mask done on a pointer difference (no pointer-to-int cast), the
 * world arena swap of codfh (dword_50F248, a port patch) is not in the exe and is left out, the structs declared here with
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
RW_LAYOUT_ASSERT(rw_assert_fl_align, RW_OFFSETOF(RwFreeList, alignment) == 0xc);
RW_LAYOUT_ASSERT(rw_assert_fl_blocks, RW_OFFSETOF(RwFreeList, blockList) == 0x10);
RW_LAYOUT_ASSERT(rw_assert_fl_size, sizeof(RwFreeList) == 0x24);

/* aligned = (p + alignment - 1) & ~(alignment - 1) on the address, via a pointer difference */
#define RW_ALIGN_PTR(p, alignment) ((RwUInt8 *)0 + ((((RwUInt8 *)(p) - (RwUInt8 *)0) + ((alignment) - 1)) & ~((alignment) - 1)))

/*007d2128*/ extern void *RwEngineInstance;

/* TIER2: unvalidated 2026-11-09 authority=diffemu */
void *_rwFreeListAllocReal(RwFreeList *freeList, RwUInt32 hint)
{
    RwUInt8 heapElement, mask;
    void *freeEntry = NULL;
    RwUInt32 i, j;
    RwUInt32 heapEntries;
    RwUInt8 *heap;
    RwLLLink *link, *lastLink;
    RwUInt32 checkEntries;

    heapEntries = freeList->heapSize;

    link = freeList->blockList.next;
    lastLink = &freeList->blockList;

    while (link != lastLink && NULL == freeEntry)
    {
        heap = (RwUInt8 *)link + sizeof(RwLLLink);

        checkEntries = freeList->entriesPerBlock;

        for (i = 0; i < heapEntries; ++i)
        {
            heapElement = heap[i];

            if (heapElement != 0xFF)
            {
                j = 0;
                while (j < 8 && checkEntries > 0)
                {
                    mask = (RwUInt8)(0x80 >> j);
                    if ((heapElement & mask) == 0)
                    {
                        RwUInt8 *aligned;

                        heap[i] = (RwUInt8)(heap[i] | mask);

                        aligned = RW_ALIGN_PTR((RwUInt8 *)link + sizeof(RwLLLink) + heapEntries, freeList->alignment);
                        freeEntry = (RwUInt8 *)aligned + (((i << 3) + j) * freeList->entrySize);
                        break;
                    }
                    ++j;
                    --checkEntries;
                }
            }
            else
            {
                checkEntries -= 8;
            }
            if (NULL != freeEntry)
                break;
        }
        link = link->next;
    }

    if (NULL == freeEntry)
    {
        RwLLLink *dataBlock;

        dataBlock = (RwLLLink *)RwMalloc(sizeof(RwLLLink) + heapEntries + (freeList->entriesPerBlock * freeList->entrySize) + freeList->alignment - 1, hint);
        if (NULL == dataBlock)
        {
            return NULL;
        }

        /* prepare new heap */
        for (i = 0; i < heapEntries; i++)
        {
            *((RwUInt8 *)dataBlock + sizeof(RwLLLink) + i) = 0;
        }

        /* rwLinkListAddLLLink(&freeList->blockList, dataBlock) */
        dataBlock->next = freeList->blockList.next;
        dataBlock->prev = &freeList->blockList;
        freeList->blockList.next->prev = dataBlock;
        freeList->blockList.next = dataBlock;

        /* allocate entry */
        *((RwUInt8 *)dataBlock + sizeof(RwLLLink)) = 0x80;

        freeEntry = RW_ALIGN_PTR((RwUInt8 *)dataBlock + sizeof(RwLLLink) + heapEntries, freeList->alignment);
    }

    return freeEntry;
}
