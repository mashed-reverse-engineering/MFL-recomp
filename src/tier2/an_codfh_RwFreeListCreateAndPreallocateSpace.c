/* RwFreeListCreateAndPreallocateSpace (MFL.exe 0x004e92a0): RenderWare Graphics 3.7 (Criterion Software), creates a free list with
 * a number of blocks preallocated (the exe reaches FreeListCreate through a jump).
 * Adapted from codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32, src/rw/src/plcore/bamemory.c @e578672,
 * not byte-identical, changes: RWAPIFUNCTION / RWASSERT / RWRETURN expanded (release build), no CODFH_WIN32 creator record,
 * the RwFreeList handle is opaque here (only passed through).  Verified by tools/diffemu (tier 2). */
#include "mfl_tier2.h"

typedef struct RwFreeList RwFreeList;

/*004e90e0*/ extern RwFreeList *FreeListCreate(RwUInt32 entrySize, RwUInt32 entriesPerBlock, RwUInt32 alignment, RwUInt32 blocks, RwFreeList *freeList, RwUInt32 hint);

/* TIER2: unvalidated 2026-11-09 authority=diffemu */
RwFreeList *RwFreeListCreateAndPreallocateSpace(RwInt32 entrySize, RwInt32 entriesPerBlock, RwInt32 alignment, RwInt32 numBlocksToPreallocate, RwFreeList *inPlaceSpaceForFreeListStruct, RwUInt32 hint)
{
    return FreeListCreate(entrySize, entriesPerBlock, alignment, numBlocksToPreallocate, inPlaceSpaceForFreeListStruct, hint);
}
