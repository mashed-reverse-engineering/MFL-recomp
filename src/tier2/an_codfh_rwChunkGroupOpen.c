/* rwChunkGroupOpen (MFL.exe 0x004eac40): RenderWare Graphics 3.7 (Criterion Software), opens the chunk group module: stores the
 * module's globals offset and creates the chunk group free list.
 * Adapted from codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32, src/rw/src/plcore/rwgrp.c @e578672,
 * not byte-identical, changes: RWFUNCTION / RWASSERT / RWRETURN expanded (release build), RWCHUNKGROUPGLOBAL expanded to the byte
 * offset into *RwEngineInstance (chunkGroupModule.globalsOffset), the chunk group sizeof (0x21), the free list block size /
 * prealloc counts and the hint (rwID_CHUNKGROUPMODULE | rwMEMHINTDUR_GLOBAL = 0x40412) as the exe has them, the module info
 * struct declared here with compile-time layout asserts.  Verified by tools/diffemu (tier 2). */
#include "mfl_tier2.h"

#define RW_OFFSETOF(type, member) ((char *)&((type *)0)->member - (char *)0)      /* pointer difference: no pointer-to-int cast */
#define RW_LAYOUT_ASSERT(name, cond) typedef char name[(sizeof(void *) != 4 || (cond)) ? 1 : -1]      /* cl 13.00 has no _Static_assert; the offsets are the 32-bit exe's */

typedef struct RwModuleInfo {
    RwInt32 globalsOffset;      /* offset of the module's globals in RwGlobals */
    RwInt32 numInstances;
} RwModuleInfo;
RW_LAYOUT_ASSERT(rw_assert_module_instances, RW_OFFSETOF(RwModuleInfo, numInstances) == 4);

/*007d2128*/ extern void *RwEngineInstance;
/*007d5194*/ extern RwModuleInfo chunkGroupModule;
/*0062e9c8*/ extern RwInt32 _rwChunkGroupFListBlockSize;
/*0062e9cc*/ extern RwInt32 _rwChunkGroupFListPreallocBlocks;
/*007d519c*/ extern char _rwChunkGroupFList[];
/*004e92a0*/ extern void *__cdecl RwFreeListCreateAndPreallocateSpace(RwInt32 size, RwInt32 blockSize, RwInt32 alignment, RwInt32 numBlocksToPrealloc, void *fl, RwUInt32 hint);

#define RWCHUNKGROUPGLOBAL_groupFList (*(void **)((RwUInt8 *)RwEngineInstance + chunkGroupModule.globalsOffset))

/* TIER2: unvalidated 2026-11-09 authority=diffemu */
void *rwChunkGroupOpen(void *instance, RwInt32 offset, RwInt32 size)
{
    /* Store the globals offset (same for all instances) */
    chunkGroupModule.globalsOffset = offset;

    /* Get on with the initialization */
    RWCHUNKGROUPGLOBAL_groupFList = RwFreeListCreateAndPreallocateSpace(0x21, _rwChunkGroupFListBlockSize, 4,
        _rwChunkGroupFListPreallocBlocks, _rwChunkGroupFList, 0x40412);
    if (!RWCHUNKGROUPGLOBAL_groupFList)
    {
        /* Failure */
        return 0;
    }

    /* One more module instance */
    ++chunkGroupModule.numInstances;

    return instance;
}
