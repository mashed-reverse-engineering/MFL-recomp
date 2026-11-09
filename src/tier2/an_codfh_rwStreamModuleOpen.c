/* rwStreamModuleOpen = codfh _rwStreamModuleOpen (MFL.exe 0x004dbde0; exe name as rwStreamModuleClose's tier-1 file): RenderWare Graphics 3.7 (Criterion Software), stream module open: creates the RwStream
 * free list in the module's globals.
 * Adapted from codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32, src/rw/src/plcore/bastream.c @e578672,
 * not byte-identical, changes: RWFUNCTION / RWRETURN expanded (release build), RWSTREAMGLOBAL / RWPLUGINOFFSET expanded to the byte
 * offset into *RwEngineInstance, sizeof(RwStream) = 0x24 and the memory hint written as constants (the exe's values), the structs
 * declared here with compile-time layout asserts.  Verified by tools/diffemu (tier 2). */
#include "mfl_tier2.h"

#define RW_OFFSETOF(type, member) ((char *)&((type *)0)->member - (char *)0)      /* pointer difference: no pointer-to-int cast */
#define RW_LAYOUT_ASSERT(name, cond) typedef char name[(sizeof(void *) != 4 || (cond)) ? 1 : -1]      /* cl 13.00 has no _Static_assert; the offsets are the 32-bit exe's */

typedef struct RwModuleInfo {
    RwInt32 globalsOffset;
    RwInt32 numInstances;
} RwModuleInfo;
typedef struct rwStreamGlobals {
    void *streamFreeList;      /* RwFreeList * */
} rwStreamGlobals;
RW_LAYOUT_ASSERT(rw_assert_module_instances, RW_OFFSETOF(RwModuleInfo, numInstances) == 4);

#define RW_SIZEOF_RWSTREAM 0x24                 /* sizeof(RwStream), PC build */
#define rwID_STREAMMODULE 0x404
#define rwMEMHINTDUR_GLOBAL 0x40000

/*007d2128*/ extern void *RwEngineInstance;
/*007d26f4*/ extern RwModuleInfo streamModule;
/*0062e838*/ extern RwInt32 _rwStreamFreeListBlockSize;
/*0062e83c*/ extern RwInt32 _rwStreamFreeListPreallocBlocks;
/*007d26d0*/ extern RwUInt8 _rwStreamFreeList[];      /* static RwFreeList storage */
/*004e92a0*/ extern void *RwFreeListCreateAndPreallocateSpace(RwInt32 entrySize, RwInt32 entriesPerBlock, RwInt32 alignment,
                                                             RwInt32 numBlocksToPreallocate, void *inPlaceSpaceForFreeListStruct, RwUInt32 hint);
#define RWSTREAMGLOBAL(var) (((rwStreamGlobals *)((RwUInt8 *)RwEngineInstance + streamModule.globalsOffset))->var)

/* TIER2: unvalidated 2026-11-09 authority=diffemu */
void *rwStreamModuleOpen(void *instance, RwInt32 offset, RwInt32 size)
{
    /* Store the globals offset (same for all instances) */
    streamModule.globalsOffset = offset;

    /* Get on with the initialization */
    RWSTREAMGLOBAL(streamFreeList) = RwFreeListCreateAndPreallocateSpace(RW_SIZEOF_RWSTREAM, _rwStreamFreeListBlockSize, sizeof(RwUInt32),
                                                                         _rwStreamFreeListPreallocBlocks, _rwStreamFreeList,
                                                                         rwID_STREAMMODULE | rwMEMHINTDUR_GLOBAL);
    if (!RWSTREAMGLOBAL(streamFreeList))
    {
        /* Failure */
        return NULL;
    }

    /* One more module instance */
    ++streamModule.numInstances;

    /* Success */
    return instance;
}
