/* rpuvanim plugin callbacks (MFL.exe 0x00561be0 - 0x00561d31): RenderWare Graphics 3.7 (Criterion Software), UV animation plugin:
 * the engine open / close (UVAnimOpen / UVAnimClose) and the material destructor (UVAnimDestructor).  RpUVAnimPluginAttach
 * stays original (it references the rw_trap rows UVAnimCopy / UVAnimWrite / UVAnimSize).
 * Adapted from codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32, src/rw/plugin/uvanim/rpuvanim.c @e578672,
 * not byte-identical, changes: RWFUNCTION / RWRETURN expanded (release build), RpUVAnimDestroy / _rpUVAnimDestruct /
 * RtAnimInterpolatorGetCurrentAnim inlined into UVAnimDestructor (the exe has them inlined; RwFreeListFree through the engine
 * function table at +0x11c), the material extension / custom data declared here by the exe's offsets with compile-time layout
 * asserts.  Verified by tools/diffemu (tier 2). */
#include "mfl_tier2.h"

#define RW_OFFSETOF(type, member) ((char *)&((type *)0)->member - (char *)0)      /* pointer difference: no pointer-to-int cast */
#define RW_LAYOUT_ASSERT(name, cond) typedef char name[(sizeof(void *) != 4 || (cond)) ? 1 : -1]      /* cl 13.00 has no _Static_assert; the offsets are the 32-bit exe's */

#define FALSE 0
#define RP_UVANIM_APPLIEDCHANNELS 2
#define RP_UVANIM_MAXSLOTS 8
#define rwID_UVANIMPLUGIN 0x135
#define RP_UVANIM_CUSTOMDATA_SIZE 0x44      /* sizeof(_rpUVAnimCustomData): name[32], nodeToUVChannelMap[8], refCount */
#define RP_UVANIM_HINT 0x40135              /* rwID_UVANIMPLUGIN | rwMEMHINTDUR_GLOBAL */

typedef struct RwModuleInfo {
    RwInt32 globalsOffset;
    RwInt32 numInstances;
} RwModuleInfo;
typedef struct rpUVAnimCustomData {
    char name[32];
    RwUInt32 nodeToUVChannelMap[RP_UVANIM_MAXSLOTS];
    RwInt32 refCount;
} rpUVAnimCustomData;
typedef struct RtAnimAnimationHead {      /* RtAnimAnimation: only the custom data pointer is used here */
    void *interpInfo;
    RwInt32 numFrames;
    RwInt32 flags;
    RwReal duration;
    void *pFrames;
    rpUVAnimCustomData *customData;
} RtAnimAnimationHead;
typedef struct RtAnimInterpolatorHead {
    RtAnimAnimationHead *pCurrentAnim;
} RtAnimInterpolatorHead;
typedef struct RpUVAnimMaterialExtension {
    void *uv[RP_UVANIM_APPLIEDCHANNELS];                    /* RwMatrix * */
    RtAnimInterpolatorHead *interp[RP_UVANIM_MAXSLOTS];
} RpUVAnimMaterialExtension;
typedef struct RpUVAnimMaterialGlobalVars {
    RwInt32 engineOffset;
} RpUVAnimMaterialGlobalVars;
RW_LAYOUT_ASSERT(rw_assert_uvanim_refcount, RW_OFFSETOF(rpUVAnimCustomData, refCount) == 0x40 && sizeof(rpUVAnimCustomData) == RP_UVANIM_CUSTOMDATA_SIZE);
RW_LAYOUT_ASSERT(rw_assert_uvanim_customdata, RW_OFFSETOF(RtAnimAnimationHead, customData) == 0x14);
RW_LAYOUT_ASSERT(rw_assert_uvanim_interp, RW_OFFSETOF(RpUVAnimMaterialExtension, interp) == 8);

/*007d2128*/ extern void *RwEngineInstance;
/*007da8b4*/ extern RwModuleInfo rpUVAnimModule;
/*00947b58*/ extern RpUVAnimMaterialGlobalVars RpUVAnimMaterialGlobals;
/*0063a384*/ extern RwInt32 _rpUVAnimCustomDataFreeListBlockSize;
/*0063a388*/ extern RwInt32 _rpUVAnimCustomDataFreeListPreallocBlocks;
/*007da8b0*/ extern void *_rpUVAnimCustomDataFreeList;
/*0063a38c*/ extern RwUInt8 _rpUVAnimDictSchema[];
/*0063a3c0*/ extern RwUInt8 _rpUVAnimLinearInterpolatorInfo[];
/*0063a3f0*/ extern RwUInt8 _rpUVAnimParamInterpolatorInfo[];
/*004e92a0*/ extern void *RwFreeListCreateAndPreallocateSpace(RwInt32 entrySize, RwInt32 entriesPerBlock, RwInt32 alignment,
                                                             RwInt32 numBlocksToPreallocate, void *inPlaceSpaceForFreeListStruct, RwUInt32 hint);
/*004e92b0*/ extern RwBool RwFreeListDestroy(void *freeList);
/*00569220*/ extern void *RtDictSchemaInit(void *schema);
/*00569250*/ extern RwBool RtDictSchemaDestruct(void *schema);
/*0053ded0*/ extern RwBool RtAnimRegisterInterpolationScheme(void *interpolatorInfo);
/*004d4e40*/ extern RwBool RwMatrixDestroy(void *matrix);
/*00573f50*/ extern RwBool RtAnimAnimationDestroy(void *animation);
/*0053e3b0*/ extern RwBool RtAnimInterpolatorDestroy(void *anim);
void *UVAnimOpen(void *instance, RwInt32 offset, RwInt32 size);
void *UVAnimClose(void *instance, RwInt32 offset, RwInt32 size);
void *UVAnimDestructor(void *object, RwInt32 offset, RwInt32 size);

typedef void (*RwFreeListFreeFn)(void *freeList, void *pData);
#define RW_FREELISTFREE_SLOT 0x11c      /* RwEngineInstance->memoryFuncs.rwfreelistfree (RwFreeListFree macro) */
#define RwFreeListFree(fl, p) (*(RwFreeListFreeFn *)((RwUInt8 *)RwEngineInstance + RW_FREELISTFREE_SLOT))((fl), (p))
#define RPUVANIMMATERIALGETDATA(material) ((RpUVAnimMaterialExtension *)((RwUInt8 *)(material) + RpUVAnimMaterialGlobals.engineOffset))

/* TIER2: unvalidated 2026-11-09 authority=diffemu */
void *UVAnimOpen(void *instance, RwInt32 offset, RwInt32 size)
{
    _rpUVAnimCustomDataFreeList = RwFreeListCreateAndPreallocateSpace(RP_UVANIM_CUSTOMDATA_SIZE, _rpUVAnimCustomDataFreeListBlockSize,
                                                                      sizeof(RwInt32), _rpUVAnimCustomDataFreeListPreallocBlocks,
                                                                      _rpUVAnimCustomDataFreeList, RP_UVANIM_HINT);
    if (NULL == _rpUVAnimCustomDataFreeList)
    {
        instance = NULL;
    }

    /* setup the dictionary schema */
    RtDictSchemaInit(_rpUVAnimDictSchema);

    RtAnimRegisterInterpolationScheme(_rpUVAnimLinearInterpolatorInfo);
    RtAnimRegisterInterpolationScheme(_rpUVAnimParamInterpolatorInfo);

    ++rpUVAnimModule.numInstances;
    return instance;
}

/* TIER2: unvalidated 2026-11-09 authority=diffemu */
void *UVAnimClose(void *instance, RwInt32 offset, RwInt32 size)
{
    --rpUVAnimModule.numInstances;

    RtDictSchemaDestruct(_rpUVAnimDictSchema);

    if (NULL != _rpUVAnimCustomDataFreeList)
    {
        RwFreeListDestroy(_rpUVAnimCustomDataFreeList);
        _rpUVAnimCustomDataFreeList = NULL;
    }
    return instance;
}

/* TIER2: unvalidated 2026-11-09 authority=diffemu */
void *UVAnimDestructor(void *object, RwInt32 offset, RwInt32 size)
{
    RpUVAnimMaterialExtension *materialExt = RPUVANIMMATERIALGETDATA(object);
    RwUInt32 i;

    for (i = 0; i < RP_UVANIM_APPLIEDCHANNELS; ++i)
    {
        void **matrix = &materialExt->uv[i];
        if (*matrix)
        {
            RwMatrixDestroy(*matrix);
        }
        *matrix = NULL;
    }

    for (i = 0; i < RP_UVANIM_MAXSLOTS; ++i)
    {
        RtAnimInterpolatorHead *interp = materialExt->interp[i];
        if (interp)
        {
            /* RpUVAnimDestroy(RtAnimInterpolatorGetCurrentAnim(interp)): the custom data is reference counted */
            RtAnimAnimationHead *anim = interp->pCurrentAnim;
            rpUVAnimCustomData *data = anim->customData;

            --data->refCount;
            if (!data->refCount)
            {
                RwFreeListFree(_rpUVAnimCustomDataFreeList, anim->customData);
                RtAnimAnimationDestroy(anim);
            }

            RtAnimInterpolatorDestroy(interp);
        }
    }
    return object;
}
