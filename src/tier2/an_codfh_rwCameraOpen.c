/* rwCameraOpen = codfh _rwCameraOpen (MFL.exe 0x004d1b10; exe name as rwCameraClose's tier-1 file): RenderWare Graphics 3.7
 * (Criterion Software), camera module open: creates the camera free list in the module's globals.
 * Adapted from codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32, src/rw/src/bacamera.c @e578672,
 * not byte-identical, changes: RWFUNCTION / RWRETURN expanded (release build, no RWDEBUG inCameraBeginEndUpdate), RWCAMERAGLOBAL /
 * RWPLUGINOFFSET expanded to the byte offset into *RwEngineInstance, the free list alignment is 4 and the hint 0x40005 (the exe's
 * values), the structs declared here with compile-time layout asserts.  Module-level only: no RwCamera object field is touched (the
 * flip blocker is the RwCamera layout).  Verified by tools/diffemu (tier 2). */
#include "mfl_tier2.h"

#define RW_OFFSETOF(type, member) ((char *)&((type *)0)->member - (char *)0)      /* pointer difference: no pointer-to-int cast */
#define RW_LAYOUT_ASSERT(name, cond) typedef char name[(sizeof(void *) != 4 || (cond)) ? 1 : -1]      /* cl 13.00 has no _Static_assert; the offsets are the 32-bit exe's */

typedef struct RwModuleInfo {
    RwInt32 globalsOffset;
    RwInt32 numInstances;
} RwModuleInfo;
typedef struct rwCameraGlobals {
    void *cameraFreeList;               /* RwFreeList * */
} rwCameraGlobals;
typedef struct RwPluginRegistryHead {
    RwInt32 sizeOfStruct;
} RwPluginRegistryHead;
RW_LAYOUT_ASSERT(rw_assert_module_instances, RW_OFFSETOF(RwModuleInfo, numInstances) == 4);

#define RW_CAMERA_FREELIST_ALIGN 4
#define RW_CAMERA_FREELIST_HINT 0x40005      /* rwMEMHINTDUR_GLOBAL | rwID_CAMERA */

/*007d2128*/ extern void *RwEngineInstance;
/*007d1ff0*/ extern RwModuleInfo cameraModule;
/*0062e534*/ extern RwPluginRegistryHead cameraTKList;
/*0062e54c*/ extern RwInt32 _rwCameraFreeListBlockSize;
/*0062e550*/ extern RwInt32 _rwCameraFreeListPreallocBlocks;
/*007d1fcc*/ extern RwUInt8 _rwCameraFreeList[];
/*004e92a0*/ extern void *RwFreeListCreateAndPreallocateSpace(RwInt32 entrySize, RwInt32 entriesPerBlock, RwInt32 alignment,
                                                             RwInt32 numBlocksToPreallocate, void *inPlaceSpaceForFreeListStruct, RwUInt32 hint);
#define RWCAMERAGLOBAL(var) (((rwCameraGlobals *)((RwUInt8 *)RwEngineInstance + cameraModule.globalsOffset))->var)

/* TIER2: unvalidated 2026-11-09 authority=diffemu */
void *rwCameraOpen(void *instance, RwInt32 offset, RwInt32 size)
{
    /* Store the data block offset (the same for all instances) */
    cameraModule.globalsOffset = offset;

    /* Create the cameras */
    RWCAMERAGLOBAL(cameraFreeList) = RwFreeListCreateAndPreallocateSpace(cameraTKList.sizeOfStruct, _rwCameraFreeListBlockSize,
                                                                         RW_CAMERA_FREELIST_ALIGN, _rwCameraFreeListPreallocBlocks,
                                                                         _rwCameraFreeList, RW_CAMERA_FREELIST_HINT);
    if (!RWCAMERAGLOBAL(cameraFreeList))
    {
        /* Failure */
        return NULL;
    }

    /* One more module instance */
    ++cameraModule.numInstances;

    /* Success */
    return instance;
}
