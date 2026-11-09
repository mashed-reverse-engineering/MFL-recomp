/* RwFreeListDestroy (MFL.exe 0x004e92b0): RenderWare Graphics 3.7 (Criterion Software), unlinks a free list and releases its blocks.
 * Adapted from codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32, src/rw/src/plcore/bamemory.c @e578672,
 * not byte-identical, changes: RWAPIFUNCTION / RWASSERT / RWRETURN expanded (release build), RwFree / RwFreeListFree expanded to
 * the engine's memoryFuncs / freeList slots, the rwLinkList macros and _RwFreeListFree inlined (as the exe has it), no CODFH_WIN32
 * creator record, the structs declared here with compile-time layout asserts.  Verified by tools/diffemu (tier 2). */
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
RW_LAYOUT_ASSERT(rw_assert_fl_blocks, RW_OFFSETOF(RwFreeList, blockList) == 0x10);
RW_LAYOUT_ASSERT(rw_assert_fl_flags, RW_OFFSETOF(RwFreeList, flags) == 0x18);
RW_LAYOUT_ASSERT(rw_assert_fl_link, RW_OFFSETOF(RwFreeList, link) == 0x1c);
RW_LAYOUT_ASSERT(rw_assert_fl_size, sizeof(RwFreeList) == 0x24);

#define rwFREELISTFLAG_STATIC 0x1

/* RwFreeListFree: RwGlobals.freeListFree 0x11c (rw37.h) */
typedef RwFreeList *(*RwFreeListFreeFunc)(RwFreeList *, void *);
#define RwFreeListFree RW_GLOBAL(0x11c, RwFreeListFreeFunc)

/*007d2128*/ extern void *RwEngineInstance;
/*007d5158*/ extern RwFreeList *_masterFreeListPtr;

/* TIER2: unvalidated 2026-11-09 authority=diffemu */
RwBool RwFreeListDestroy(RwFreeList *freeList)
{
    RwLLLink *link;

    /* remove from list of freelists */
    freeList->link.prev->next = freeList->link.next;
    freeList->link.next->prev = freeList->link.prev;

    /* _RwFreeListFree(freeList) */
    link = freeList->blockList.next;
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
    return 1;      /* TRUE */
}
