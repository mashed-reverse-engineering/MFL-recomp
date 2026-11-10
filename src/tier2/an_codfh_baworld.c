/* baworld module (MFL.exe 0x004f5fc0 / 0x004f6010): RenderWare Graphics 3.7 (Criterion Software), world module close / open
 * (WorldClose / WorldOpen): the world pipelines, the listed-world free list and the world list head.
 * Adapted from codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32, src/rw/world/baworld.c @e578672,
 * not byte-identical, changes: RWFUNCTION / RWRETURN expanded (release build), RPWORLDGLOBAL expanded to the byte offset into
 * *RwEngineInstance, rwLinkListInitialize expanded, sizeof(RpListedWorld) written as its exe value 0x10, the globals
 * declared here with compile-time layout asserts.  Module-level only: no RpWorld field is touched.  Verified by tools/diffemu (tier 2). */
#include "mfl_tier2.h"

#define RW_OFFSETOF(type, member) ((char *)&((type *)0)->member - (char *)0)      /* pointer difference: no pointer-to-int cast */
#define RW_LAYOUT_ASSERT(name, cond) typedef char name[(sizeof(void *) != 4 || (cond)) ? 1 : -1]      /* cl 13.00 has no _Static_assert; the offsets are the 32-bit exe's */

#define rpLISTEDWORLDSIZE 0x10              /* sizeof(RpListedWorld) */
#define rpWORLDLIST_HINT 0x4000b            /* rwMEMHINTDUR_GLOBAL | rwID_WORLD */

typedef struct RwModuleInfo {
    RwInt32 globalsOffset;
    RwInt32 numInstances;
} RwModuleInfo;
typedef struct RwLLLink {
    struct RwLLLink *next;
    struct RwLLLink *prev;
} RwLLLink;
typedef struct rpWorldGlobals {
    void *worldListFreeList;                /* RwFreeList * */
    RwLLLink worldListHead;                 /* RwLinkList */
} rpWorldGlobals;
RW_LAYOUT_ASSERT(rw_assert_world_listhead, RW_OFFSETOF(rpWorldGlobals, worldListHead) == 4);

/*007d2128*/ extern void *RwEngineInstance;
/*007d52fc*/ extern RwModuleInfo worldModule;
/*007d52d8*/ extern RwUInt8 _rpWorldListFreeList[];
/*004e92a0*/ extern void *RwFreeListCreateAndPreallocateSpace(RwInt32 entrySize, RwInt32 entriesPerBlock, RwInt32 alignment,
                                                             RwInt32 numBlocksToPreallocate, void *inPlaceSpaceForFreeListStruct, RwUInt32 hint);
/*004e92b0*/ extern RwBool RwFreeListDestroy(void *freeList);
/*00500a40*/ extern RwBool _rpWorldPipelineOpen(void);
/*00500a30*/ extern RwBool _rpWorldPipelineClose(void);
#define RPWORLDGLOBAL(var) (((rpWorldGlobals *)((RwUInt8 *)RwEngineInstance + worldModule.globalsOffset))->var)

/* TIER2: unvalidated 2026-11-09 authority=diffemu */
void *WorldClose(void *instance, RwInt32 offset, RwInt32 size)
{
    if (RPWORLDGLOBAL(worldListFreeList))
    {
        RwFreeListDestroy(RPWORLDGLOBAL(worldListFreeList));
        RPWORLDGLOBAL(worldListFreeList) = NULL;
    }

    _rpWorldPipelineClose();

    /* One less module instance */
    --worldModule.numInstances;
    return instance;
}

/* TIER2: unvalidated 2026-11-09 authority=diffemu */
void *WorldOpen(void *instance, RwInt32 offset, RwInt32 size)
{
    /* Grab the global data offset (same for all instances) */
    worldModule.globalsOffset = offset;

    if (!_rpWorldPipelineOpen())
    {
        return NULL;
    }

    /* Can't see many of these being allocated */
    RPWORLDGLOBAL(worldListFreeList) = RwFreeListCreateAndPreallocateSpace(rpLISTEDWORLDSIZE, 8, sizeof(RwUInt32), 1,
                                                                           _rpWorldListFreeList, rpWORLDLIST_HINT);
    if (!RPWORLDGLOBAL(worldListFreeList))
    {
        /* Failure */
        return NULL;
    }

    /* rwLinkListInitialize(&RPWORLDGLOBAL(worldListHead)) */
    RPWORLDGLOBAL(worldListHead).next = &RPWORLDGLOBAL(worldListHead);
    RPWORLDGLOBAL(worldListHead).prev = &RPWORLDGLOBAL(worldListHead);

    /* One more module instance */
    ++worldModule.numInstances;
    return instance;
}
