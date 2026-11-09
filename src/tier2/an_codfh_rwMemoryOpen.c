/* rwMemoryOpen (MFL.exe 0x004e96e0): RenderWare Graphics 3.7 (Criterion Software), opens the free list module (creates the master
 * free list) and installs the memory functions (the user's or the ANSI ones).
 * Adapted from codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32, src/rw/src/plcore/bamemory.c @e578672,
 * not byte-identical, changes: RWFUNCTION / RWASSERT / RWRETURN expanded (release build), _rwFreeListModuleOpen inlined (as the exe
 * has it), the master free list uses alignment 4 (the exe's rwMALLOCALIGNMENT; codfh 16), RWSRCGLOBAL(memoryFuncs) expanded to the
 * byte offset 0x108 into *RwEngineInstance, the structs declared here with compile-time layout asserts.
 * Verified by tools/diffemu (tier 2). */
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
struct RwMemoryFunctions {
    RwMallocFunc rwmalloc;
    RwFreeFunc rwfree;
    void *(*rwrealloc)(void *, size_t, RwUInt32);
    void *(*rwcalloc)(size_t, size_t, RwUInt32);
};
RW_LAYOUT_ASSERT(rw_assert_link_size, sizeof(RwLLLink) == 8);
RW_LAYOUT_ASSERT(rw_assert_fl_link, RW_OFFSETOF(RwFreeList, link) == 0x1c);
RW_LAYOUT_ASSERT(rw_assert_fl_size, sizeof(RwFreeList) == 0x24);
RW_LAYOUT_ASSERT(rw_assert_mf_size, sizeof(RwMemoryFunctions) == 0x10);
RW_LAYOUT_ASSERT(rw_assert_mf_free, RW_OFFSETOF(RwMemoryFunctions, rwfree) == 4);
RW_LAYOUT_ASSERT(rw_assert_mf_realloc, RW_OFFSETOF(RwMemoryFunctions, rwrealloc) == 8);
RW_LAYOUT_ASSERT(rw_assert_mf_calloc, RW_OFFSETOF(RwMemoryFunctions, rwcalloc) == 0xc);

#define rwMEMHINTDUR_GLOBAL 0x00040000
#define rwMALLOCALIGNMENT 4
#define RWSRC_MEMORYFUNCS ((RwMemoryFunctions *)((char *)RwEngineInstance + 0x108))      /* RwGlobals.memoryFuncs (rw37.h) */

/*007d2128*/ extern void *RwEngineInstance;
/*007d5128*/ extern RwLLLink _freeListList;
/*007d5130*/ extern RwFreeList _masterFreeList;
/*007d5154*/ extern RwBool _freeListModuleOpen;
/*007d5158*/ extern RwFreeList *_masterFreeListPtr;
/*004e90e0*/ extern RwFreeList *FreeListCreate(RwUInt32 entrySize, RwUInt32 entriesPerBlock, RwUInt32 alignment, RwUInt32 blocks, RwFreeList *freeList, RwUInt32 hint);
/*004e97b0*/ extern void *HMalloc(size_t size, RwUInt32 hint);
/*004b45fd*/ extern void free(void *mem);
/*004e97c0*/ extern void *HRealloc(void *mem, size_t newSize, RwUInt32 hint);
/*004e97e0*/ extern void *HCalloc(size_t numObj, size_t sizeObj, RwUInt32 hint);

/* TIER2: unvalidated 2026-11-09 authority=diffemu */
RwBool _rwMemoryOpen(const RwMemoryFunctions *memFuncs)
{
    /* _rwFreeListModuleOpen() */
    _freeListList.next = &_freeListList;
    _freeListList.prev = &_freeListList;
    _freeListModuleOpen = 1;

    _masterFreeListPtr = FreeListCreate(sizeof(RwFreeList), 16, rwMALLOCALIGNMENT, 0, &_masterFreeList, rwMEMHINTDUR_GLOBAL);
    if (NULL == _masterFreeListPtr)
    {
        _freeListModuleOpen = 0;
        return 0;      /* FALSE */
    }
    /* remove the master free list from the list of free lists */
    _masterFreeListPtr->link.prev->next = _masterFreeListPtr->link.next;
    _masterFreeListPtr->link.next->prev = _masterFreeListPtr->link.prev;

    if (memFuncs)
    {
        *RWSRC_MEMORYFUNCS = *memFuncs;
    }
    else
    {
        RWSRC_MEMORYFUNCS->rwmalloc = HMalloc;
        RWSRC_MEMORYFUNCS->rwfree = free;
        RWSRC_MEMORYFUNCS->rwrealloc = HRealloc;
        RWSRC_MEMORYFUNCS->rwcalloc = HCalloc;
    }
    return 1;      /* TRUE */
}
