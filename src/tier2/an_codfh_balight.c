/* balight module (MFL.exe 0x004f4f80 / 0x004f5020): RenderWare Graphics 3.7 (Criterion Software), light module close / open:
 * the light free list (rpLightClose / rpLightOpen = codfh _rpLightClose / _rpLightOpen).
 * Adapted from codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32, src/rw/world/balight.c @e578672,
 * not byte-identical, changes: RWFUNCTION / RWRETURN expanded (release build), RWLIGHTGLOBAL / RWPLUGINOFFSET expanded to the byte
 * offset into *RwEngineInstance, free list alignment 4 and hint 0x40012 (the exe's values), the structs declared here with
 * compile-time layout asserts.  Module-level only: RpLight objects are only passed to LightTidyDestroyLight (original code), no field
 * is touched (the flip blocker is the RpLight size).  Verified by tools/diffemu (tier 2). */
#include "mfl_tier2.h"

#define RW_OFFSETOF(type, member) ((char *)&((type *)0)->member - (char *)0)      /* pointer difference: no pointer-to-int cast */
#define RW_LAYOUT_ASSERT(name, cond) typedef char name[(sizeof(void *) != 4 || (cond)) ? 1 : -1]      /* cl 13.00 has no _Static_assert; the offsets are the 32-bit exe's */

typedef struct RwModuleInfo {
    RwInt32 globalsOffset;
    RwInt32 numInstances;
} RwModuleInfo;
typedef struct rpLightGlobals {
    void *lightFreeList;                /* RwFreeList * */
} rpLightGlobals;
typedef struct RwPluginRegistryHead {
    RwInt32 sizeOfStruct;
} RwPluginRegistryHead;
RW_LAYOUT_ASSERT(rw_assert_module_instances, RW_OFFSETOF(RwModuleInfo, numInstances) == 4);

#define RP_LIGHT_FREELIST_ALIGN 4
#define RP_LIGHT_FREELIST_HINT 0x40012      /* rwMEMHINTDUR_GLOBAL | rwID_LIGHT */

/*007d2128*/ extern void *RwEngineInstance;
/*007d52d0*/ extern RwModuleInfo lightModule;
/*0062ebcc*/ extern RwPluginRegistryHead lightTKList;
/*0062ebe4*/ extern RwInt32 _rpLightFreeListBlockSize;
/*0062ebe8*/ extern RwInt32 _rpLightFreeListPreallocBlocks;
/*007d52ac*/ extern RwUInt8 _rpLightFreeList[];
/*004f4fe0*/ extern void LightTidyDestroyLight(void *mem, void *data);
/*004e95a0*/ extern void *RwFreeListForAllUsed(void *freeList, void *fpCallBack, void *data);
/*004e92b0*/ extern RwBool RwFreeListDestroy(void *freeList);
/*004e92a0*/ extern void *RwFreeListCreateAndPreallocateSpace(RwInt32 entrySize, RwInt32 entriesPerBlock, RwInt32 alignment,
                                                             RwInt32 numBlocksToPreallocate, void *inPlaceSpaceForFreeListStruct, RwUInt32 hint);
#define RWLIGHTGLOBAL(var) (((rpLightGlobals *)((RwUInt8 *)RwEngineInstance + lightModule.globalsOffset))->var)

/* TIER2: unvalidated 2026-11-09 authority=diffemu */
void *rpLightClose(void *instance, RwInt32 offset, RwInt32 size)
{
    /* Destroy all the lights still allocated, then the free list */
    RwFreeListForAllUsed(RWLIGHTGLOBAL(lightFreeList), (void *)LightTidyDestroyLight, NULL);
    RwFreeListDestroy(RWLIGHTGLOBAL(lightFreeList));
    RWLIGHTGLOBAL(lightFreeList) = NULL;

    /* One less module instance */
    --lightModule.numInstances;
    return instance;
}

/* TIER2: unvalidated 2026-11-09 authority=diffemu */
void *rpLightOpen(void *instance, RwInt32 offset, RwInt32 size)
{
    /* Store the data block offset (the same for all instances) */
    lightModule.globalsOffset = offset;

    RWLIGHTGLOBAL(lightFreeList) = RwFreeListCreateAndPreallocateSpace(lightTKList.sizeOfStruct, _rpLightFreeListBlockSize,
                                                                       RP_LIGHT_FREELIST_ALIGN, _rpLightFreeListPreallocBlocks,
                                                                       _rpLightFreeList, RP_LIGHT_FREELIST_HINT);
    if (RWLIGHTGLOBAL(lightFreeList))
    {
        /* One more module instance */
        ++lightModule.numInstances;
        return instance;
    }
    return NULL;
}
