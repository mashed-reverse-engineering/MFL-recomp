/* rpskin plugin callbacks (MFL.exe 0x0053e930 - 0x0053ebbd, 0x00540600): RenderWare Graphics 3.7 (Criterion Software), skin plugin:
 * the engine open / close (SkinOpen / SkinClose), the geometry destructor, the atomic constructor, the atomic stream "always"
 * callback and _rpSkinSplitDataDestroy (an rw_trap row, ported so the destructor does not reference int3 code).
 * RpSkinPluginAttach stays original: its geometry stream writer / size / rights callbacks are rw_trap rows with trapped callees.
 * Adapted from codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32, src/rw/plugin/skin2/rpskin.c @e578672
 * and src/rw/plugin/skin2/bsplit.c @e578672, not byte-identical, changes: RWFUNCTION / RWRETURN / RWASSERT expanded (release build), MatfxPluginIsAttached / ToonPluginIsAttached / RpSkinDestroy / SkinAtomicSetup /
 * SkinAtomicAttachBestPipeForAttachedPlugins inlined (as in the exe; RwDebugSendMessage is not in the release build),
 * RwMalloc / RwFree / RwFreeListFree through the engine function table (+0x108 / +0x10c / +0x11c), the exe folds
 * SkinAtomicWrite, SkinGeometryCopy and _rpSkinDeinitialize into one `return first argument` function (0x0053eac0), the
 * globals / RpSkin fields declared here by the exe's offsets with compile-time layout asserts.  Verified by tools/diffemu (tier 2). */
#include "mfl_tier2.h"

#define RW_OFFSETOF(type, member) ((char *)&((type *)0)->member - (char *)0)      /* pointer difference: no pointer-to-int cast */
#define RW_LAYOUT_ASSERT(name, cond) typedef char name[(sizeof(void *) != 4 || (cond)) ? 1 : -1]      /* cl 13.00 has no _Static_assert; the offsets are the 32-bit exe's */

#define TRUE 1
#define rwID_SKINPLUGIN 0x116
#define rwID_MATERIALEFFECTSPLUGIN 0x120
#define rwID_TOONPLUGIN 0x12e
#define rwSKIN_HINT 0x40116                 /* rwID_SKINPLUGIN | rwMEMHINTDUR_GLOBAL */
#define rpSKINPIPELINESKINGENERIC 1
#define rpSKINPIPELINESKINMATFX 2
#define rpSKINPIPELINESKINTOON 4
#define rpSKINTYPEGENERIC 1
#define rpSKINTYPEMATFX 2
#define rpSKINTYPETOON 3
#define rpSKINALIGNMENT 4
#define rpSKINSIZE 0x40                     /* sizeof(RpSkin) */
#define rpSKINMAXNUMBEROFMATRICES 256
#define rwMATRIXSIZE 0x40

typedef struct RwModuleInfo {
    RwInt32 globalsOffset;
    RwInt32 numInstances;
} RwModuleInfo;
typedef struct SkinMatrixCache {
    void *aligned;                          /* RwMatrix * */
    void *unaligned;
} SkinMatrixCache;
typedef struct SkinGlobals {
    RwInt32 engineOffset;
    RwInt32 atomicOffset;
    RwInt32 geometryOffset;
    SkinMatrixCache matrixCache;
    RwUInt32 numMatrices;
    void *freeList;                         /* RwFreeList * */
    RwModuleInfo module;
} SkinGlobals;
typedef struct SkinSplitData {
    RwUInt32 boneLimit;
    RwUInt32 numMeshes;
    RwUInt32 numRLE;
    RwUInt8 *matrixRemapIndices;
    void *meshRLECount;
    void *meshRLE;
} SkinSplitData;
typedef struct RpSkinHead {                 /* RpSkin: the split data and the extension data pointer are used here */
    RwUInt8 pad[0x24];
    SkinSplitData skinSplitData;
    void *unaligned;
} RpSkinHead;
typedef struct RpAtomicHead {               /* RpAtomic: only the geometry pointer is used here */
    RwUInt8 pad[0x18];
    void *geometry;
} RpAtomicHead;
RW_LAYOUT_ASSERT(rw_assert_skin_freelist, RW_OFFSETOF(SkinGlobals, freeList) == 0x18 && RW_OFFSETOF(SkinGlobals, module) == 0x1c);
RW_LAYOUT_ASSERT(rw_assert_skin_unaligned, RW_OFFSETOF(RpSkinHead, skinSplitData) == 0x24 && RW_OFFSETOF(RpSkinHead, unaligned) == 0x3c);
RW_LAYOUT_ASSERT(rw_assert_atomic_geometry, RW_OFFSETOF(RpAtomicHead, geometry) == 0x18);

/*007d2128*/ extern void *RwEngineInstance;
/*007da640*/ extern SkinGlobals _rpSkinGlobals;
/*0063a19c*/ extern RwInt32 _rpSkinFreeListBlockSize;
/*0063a1a0*/ extern RwInt32 _rpSkinFreeListPreallocBlocks;
/*007d9e1c*/ extern RwUInt8 _rpSkinFreeList[];
/*004d2f50*/ extern RwInt32 RwEngineGetPluginOffset(RwUInt32 pluginID);
/*004f7f50*/ extern RwInt32 RpAtomicGetPluginOffset(RwUInt32 pluginID);
/*004e92a0*/ extern void *RwFreeListCreateAndPreallocateSpace(RwInt32 entrySize, RwInt32 entriesPerBlock, RwInt32 alignment,
                                                             RwInt32 numBlocksToPreallocate, void *inPlaceSpaceForFreeListStruct, RwUInt32 hint);
/*004e92b0*/ extern RwBool RwFreeListDestroy(void *freeList);
/*00540540*/ extern RwBool _rpSkinPipelinesCreate(RwUInt32 pipes);
/*005402d0*/ extern RwBool _rpSkinPipelinesDestroy(void);
/*00540570*/ extern void *rpSkinPipelinesAttach(void *atomic, RwInt32 type);
/*0053eac0*/ extern void *_rpSkinDeinitialize(void *geometry);
RwBool _rpSkinSplitDataDestroy(RpSkinHead *skin);
void *SkinOpen(void *instance, RwInt32 offset, RwInt32 size);
void *SkinClose(void *instance, RwInt32 offset, RwInt32 size);
void *SkinGeometryDestructor(void *object, RwInt32 offset, RwInt32 size);
void *SkinAtomicConstructor(void *object, RwInt32 offset, RwInt32 size);
RwBool SkinAtomicAlways(void *object, RwInt32 offset, RwInt32 size);

typedef void (*RwFreeListFreeFn)(void *freeList, void *pData);
#define RwFreeListFree(fl, p) (*(RwFreeListFreeFn *)((RwUInt8 *)RwEngineInstance + 0x11c))((fl), (p))      /* memoryFuncs.rwfreelistfree */
#define RPSKINGEOMETRYGETDATA(geometry) ((RpSkinHead **)((RwUInt8 *)(geometry) + _rpSkinGlobals.geometryOffset))
#define RPSKINATOMICGETDATA(atomic) ((void **)((RwUInt8 *)(atomic) + _rpSkinGlobals.atomicOffset))      /* SkinAtomicData: the hierarchy pointer */
#define MatfxPluginIsAttached() (RwEngineGetPluginOffset(rwID_MATERIALEFFECTSPLUGIN) != -1)
#define ToonPluginIsAttached() (RwEngineGetPluginOffset(rwID_TOONPLUGIN) != -1)
#define ROUNDUP16(p) ((void *)(((RwUInt8 *)(p) + 15) - (((RwUInt8 *)(p) + 15) - (RwUInt8 *)0 & 15)))      /* pointer arithmetic only: no pointer-to-int cast */

/* TIER2: unvalidated 2026-11-09 authority=diffemu */
void *SkinOpen(void *instance, RwInt32 offset, RwInt32 size)
{
    /* Create the skinning pipelines, only once. */
    if (_rpSkinGlobals.module.numInstances == 0)
    {
        RwUInt32 pipes;
        RwUInt32 cacheSize;

        /* Assume we always want the generic pipe. */
        pipes = rpSKINPIPELINESKINGENERIC;

        /* Is the matfx plugin attached? */
        if (MatfxPluginIsAttached())
        {
            pipes |= rpSKINPIPELINESKINMATFX;
        }

        /* Is the toon plugin attached? */
        if (ToonPluginIsAttached())
        {
            pipes |= rpSKINPIPELINESKINTOON;
        }

        /* Set up the pipeline. */
        _rpSkinPipelinesCreate(pipes);

        /* Create a RpSkin freelist. */
        _rpSkinGlobals.freeList = RwFreeListCreateAndPreallocateSpace(rpSKINSIZE, _rpSkinFreeListBlockSize, rpSKINALIGNMENT,
                                                                      _rpSkinFreeListPreallocBlocks, _rpSkinFreeList, rwSKIN_HINT);

        /* Create the skinning matrix cache. */
        cacheSize = (rwMATRIXSIZE * rpSKINMAXNUMBEROFMATRICES) + 15;
        _rpSkinGlobals.matrixCache.unaligned = RwMalloc(cacheSize, rwSKIN_HINT);

        /* Clean the memory. */
        memset(_rpSkinGlobals.matrixCache.unaligned, 0, cacheSize);

        _rpSkinGlobals.matrixCache.aligned = ROUNDUP16(_rpSkinGlobals.matrixCache.unaligned);
    }

    /* Another instance. */
    _rpSkinGlobals.module.numInstances++;
    return instance;
}

/* TIER2: unvalidated 2026-11-09 authority=diffemu */
void *SkinClose(void *instance, RwInt32 offset, RwInt32 size)
{
    /* One less module instance. */
    _rpSkinGlobals.module.numInstances--;

    if (0 == _rpSkinGlobals.module.numInstances)
    {
        /* Destroy the skinning pipelines. */
        _rpSkinPipelinesDestroy();

        /* Destroy the skin free list. */
        RwFreeListDestroy(_rpSkinGlobals.freeList);
        _rpSkinGlobals.freeList = NULL;

        /* Destroy the matrix cache. */
        RwFree(_rpSkinGlobals.matrixCache.unaligned);
        _rpSkinGlobals.matrixCache.unaligned = NULL;
    }
    return instance;
}

/* TIER2: unvalidated 2026-11-09 authority=diffemu */
void *SkinGeometryDestructor(void *object, RwInt32 offset, RwInt32 size)
{
    RpSkinHead *skin = *RPSKINGEOMETRYGETDATA(object);

    if (NULL != skin)
    {
        /* Give the platforms a chance to destroy the skin. */
        _rpSkinDeinitialize(object);

        /* Destroy the skin (RpSkinDestroy), and remove the extension. */
        if (skin->unaligned)
        {
            RwFree(skin->unaligned);
        }
        _rpSkinSplitDataDestroy(skin);
        RwFreeListFree(_rpSkinGlobals.freeList, skin);
        *RPSKINGEOMETRYGETDATA(object) = NULL;
    }
    return object;
}

/* TIER2: unvalidated 2026-11-09 authority=diffemu */
void *SkinAtomicConstructor(void *object, RwInt32 offset, RwInt32 size)
{
    /* Clean the memory */
    *RPSKINATOMICGETDATA(object) = NULL;
    return object;
}

/* TIER2: unvalidated 2026-11-09 authority=diffemu */
RwBool SkinAtomicAlways(void *object, RwInt32 offset, RwInt32 size)
{
    /* Pre-rights streaming matfxed skinned atomic legacy compatibility: assume generic unless the matfx flag is set. */
    RwInt32 type = rpSKINTYPEGENERIC;
    RpAtomicHead *atomic = (RpAtomicHead *)object;

    if (MatfxPluginIsAttached() && *((RwUInt8 *)atomic + RpAtomicGetPluginOffset(rwID_MATERIALEFFECTSPLUGIN)))
    {
        type = rpSKINTYPEMATFX;
    }

    /* SkinAtomicSetup */
    if (NULL != atomic->geometry && NULL != *RPSKINGEOMETRYGETDATA(atomic->geometry))
    {
        /* SkinAtomicAttachBestPipeForAttachedPlugins: demote to generic if the plugins are missing */
        if (!MatfxPluginIsAttached() && (type == rpSKINTYPEMATFX))
        {
            type = rpSKINTYPEGENERIC;
        }
        else if (!ToonPluginIsAttached() && (type == rpSKINTYPETOON))
        {
            type = rpSKINTYPEGENERIC;
        }
        rpSkinPipelinesAttach(atomic, type);
    }
    return TRUE;
}

/* TIER2: unvalidated 2026-11-09 authority=diffemu */
RwBool _rpSkinSplitDataDestroy(RpSkinHead *skin)
{
    SkinSplitData *skinSplitData = &skin->skinSplitData;

    if (skinSplitData->matrixRemapIndices != NULL)
    {
        RwFree(skinSplitData->matrixRemapIndices);
    }

    skinSplitData->boneLimit = 0;
    skinSplitData->numMeshes = 0;
    skinSplitData->numRLE = 0;

    skinSplitData->matrixRemapIndices = NULL;
    skinSplitData->meshRLECount = NULL;
    skinSplitData->meshRLE = NULL;
    return TRUE;
}
