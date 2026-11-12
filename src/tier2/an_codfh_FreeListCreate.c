/* FreeListCreate (MFL.exe 0x004e90e0): RenderWare Graphics 3.7 (Criterion Software), creates a free list (and its first blocks).
 * Adapted from codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32, src/rw/src/plcore/bamemory.c @e578672,
 * not byte-identical, changes: RWFUNCTION / RWASSERT / RWRETURN / RWMESSAGE expanded (release build), RwMalloc / RwFree /
 * RwFreeListAlloc / RwFreeListFree expanded to the engine's memoryFuncs / freeList slots, the rwLinkList macros and
 * _RwFreeListFree inlined, the default alignment is 4 (the exe's rwMALLOCALIGNMENT; codfh 16), no CODFH_WIN32 creator record,
 * the structs declared here with compile-time layout asserts.  Verified by tools/diffemu (tier 2). */
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
RW_LAYOUT_ASSERT(rw_assert_fl_flags, RW_OFFSETOF(RwFreeList, flags) == 0x18);
RW_LAYOUT_ASSERT(rw_assert_fl_link, RW_OFFSETOF(RwFreeList, link) == 0x1c);
RW_LAYOUT_ASSERT(rw_assert_fl_size, sizeof(RwFreeList) == 0x24);

#define rwFREELISTFLAG_STATIC 0x1
#define rwFREELISTFLAG_FREEBLOCKS 0x2
#define rwMALLOCALIGNMENT 4
#define RwMemoryHintGetDuration(hint) ((hint) & 0x00ff0000)

/* RwFreeListAlloc / RwFreeListFree: RwGlobals.freeListAlloc 0x118 / freeListFree 0x11c (rw37.h) */
typedef void *(*RwFreeListAllocFunc)(RwFreeList *, RwUInt32);
typedef RwFreeList *(*RwFreeListFreeFunc)(RwFreeList *, void *);
#define RwFreeListAlloc RW_GLOBAL(0x118, RwFreeListAllocFunc)
#define RwFreeListFree RW_GLOBAL(0x11c, RwFreeListFreeFunc)

/*007d2128*/ extern void *RwEngineInstance;
/*0062e9bc*/ extern RwBool FreeListsEnabled;
/*007d5128*/ extern RwLLLink _freeListList;
/*007d5158*/ extern RwFreeList *_masterFreeListPtr;

/* TIER2: validated 2026-11-10 authority=diffemu */
RwFreeList *FreeListCreate(RwUInt32 entrySize, RwUInt32 entriesPerBlock, RwUInt32 alignment, RwUInt32 blocks, RwFreeList *freeList, RwUInt32 hint)
{
    RwUInt32 alignedBlockSize;
    RwUInt32 heapSize;

    /* Don't allocate space if freelists disabled */
    if (!FreeListsEnabled)
    {
        blocks = 0;
    }

    if (alignment == 0)
    {
        alignment = rwMALLOCALIGNMENT;
    }

    if (NULL == freeList)
    {
        if (NULL != _masterFreeListPtr)
        {
            freeList = (RwFreeList *)RwFreeListAlloc(_masterFreeListPtr, RwMemoryHintGetDuration(hint));
        }
        else
        {
            freeList = (RwFreeList *)RwMalloc(sizeof(RwFreeList), RwMemoryHintGetDuration(hint));
        }
        if (NULL == freeList)
        {
            return NULL;
        }
        freeList->flags = rwFREELISTFLAG_FREEBLOCKS;
    }
    else
    {
        freeList->flags = rwFREELISTFLAG_STATIC | rwFREELISTFLAG_FREEBLOCKS;
    }

    alignedBlockSize = (entrySize + (alignment - 1)) & ~(alignment - 1);
    heapSize = (entriesPerBlock + 7) & ~(RwUInt32)7;
    heapSize >>= 3;

    freeList->entrySize = alignedBlockSize;
    freeList->entriesPerBlock = entriesPerBlock;
    freeList->alignment = alignment;
    freeList->heapSize = heapSize;

    /* rwLinkListInitialize(&freeList->blockList) */
    freeList->blockList.next = &freeList->blockList;
    freeList->blockList.prev = &freeList->blockList;

    while (blocks > 0)
    {
        RwLLLink *block;

        block = (RwLLLink *)RwMalloc(sizeof(RwLLLink) + heapSize + (entriesPerBlock * alignedBlockSize) + alignment - 1, hint);
        if (NULL == block)
        {
            /* _RwFreeListFree(freeList) */
            RwLLLink *link = freeList->blockList.next;

            while (link != &freeList->blockList)
            {
                link->prev->next = link->next;
                link->next->prev = link->prev;
                RwFree(link);
                link = freeList->blockList.next;
            }
            if (!(freeList->flags & rwFREELISTFLAG_STATIC))
            {
                if (_masterFreeListPtr == freeList || _masterFreeListPtr == NULL)
                {
                    RwFree(freeList);
                }
                else
                {
                    RwFreeListFree(_masterFreeListPtr, freeList);
                }
            }
            return NULL;
        }

        /* rwLLLinkInitialize + rwLinkListAddLLLink(&freeList->blockList, block) */
        block->next = NULL;
        block->prev = NULL;
        block->next = freeList->blockList.next;
        block->prev = &freeList->blockList;
        freeList->blockList.next->prev = block;
        freeList->blockList.next = block;

        /* setup heap */
        {
            RwUInt8 *heap = (RwUInt8 *)block + sizeof(RwLLLink);
            RwUInt32 i;

            for (i = 0; i < heapSize; i++)
            {
                heap[i] = 0;
            }
        }
        --blocks;
    }

    /* add to the list of free lists */
    freeList->link.next = _freeListList.next;
    freeList->link.prev = &_freeListList;
    _freeListList.next->prev = &freeList->link;
    _freeListList.next = &freeList->link;

    return freeList;
}
