/* rpuvanim plugin attach + material copy / stream writers (MFL.exe 0x00561b70, 0x00561d40 - 0x00561eb6, 0x005621c0, 0x0053e4a0):
 * RenderWare Graphics 3.7 (Criterion Software), UV animation plugin: RpUVAnimPluginAttach, UVAnimCopy, UVAnimWrite, UVAnimSize and the
 * rtanim toolkit's RtAnimInterpolatorCopy (all four rw_trap rows, ported so the attach does not reference int3 code).
 * Adapted from codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32, src/rw/plugin/uvanim/rpuvanim.c @e578672 and
 * src/rw/tool/anim/rtanim.c @e578672, not byte-identical, changes: RWFUNCTION / RWRETURN expanded (release build), the 3.7 UVAnimCopy
 * of the exe (a new interpolator per slot, copied, the animation reference counted; on failure the slots done so far are released;
 * codfh has the 3.6 copy of the extension), RpUVAnimDestroy / RpUVAnimAddRef / RpUVAnimGetName / RpMaterialUVAnimExists /
 * RtAnimInterpolatorGetCurrentAnim inlined (as in the exe), the material extension / custom data declared here by the exe's offsets
 * with compile-time layout asserts.  Verified by tools/diffemu (tier 2). */
#include "mfl_tier2.h"

#define RW_OFFSETOF(type, member) ((char *)&((type *)0)->member - (char *)0)      /* pointer difference: no pointer-to-int cast */
#define RW_LAYOUT_ASSERT(name, cond) typedef char name[(sizeof(void *) != 4 || (cond)) ? 1 : -1]      /* cl 13.00 has no _Static_assert; the offsets are the 32-bit exe's */

#define TRUE 1
#define FALSE 0
#define RP_UVANIM_APPLIEDCHANNELS 2
#define RP_UVANIM_MAXSLOTS 8
#define RP_UVANIM_MAXNAME 32
#define rwID_STRUCT 0x01
#define rwID_UVANIMPLUGIN 0x135
#define rwLIBRARYCURRENTVERSION 0x37002
#define rwLIBRARYBUILDNUM 0x24
#define rwCHUNKHEADERSIZE 12

typedef struct rpUVAnimCustomData {
    char name[RP_UVANIM_MAXNAME];
    RwUInt32 nodeToUVChannelMap[RP_UVANIM_MAXSLOTS];
    RwInt32 refCount;
} rpUVAnimCustomData;
typedef struct RtAnimInterpolatorInfoHead {      /* RtAnimInterpolatorInfo: typeID, interpKeyFrameSize */
    RwInt32 typeID;
    RwInt32 interpKeyFrameSize;
} RtAnimInterpolatorInfoHead;
typedef struct RtAnimAnimationHead {      /* RtAnimAnimation up to the custom data pointer */
    RtAnimInterpolatorInfoHead *interpInfo;
    RwInt32 numFrames;
    RwInt32 flags;
    RwReal duration;
    void *pFrames;
    rpUVAnimCustomData *customData;
} RtAnimAnimationHead;
typedef struct RtAnimInterpolatorHead {   /* RtAnimInterpolator up to the first interpolated key frame */
    RtAnimAnimationHead *pCurrentAnim;
    RwUInt8 pad04[0x20];
    RwInt32 currentInterpKeyFrameSize;
    RwUInt8 pad28[4];
    RwInt32 numNodes;
    RwUInt8 pad30[0x1c];
    RwUInt8 frames[4];                    /* rtANIMGETINTERPFRAME(anim, 0) */
} RtAnimInterpolatorHead;
typedef struct RpUVAnimMaterialExtension {
    void *uv[RP_UVANIM_APPLIEDCHANNELS];                    /* RwMatrix * */
    RtAnimInterpolatorHead *interp[RP_UVANIM_MAXSLOTS];
} RpUVAnimMaterialExtension;
typedef struct RpUVAnimMaterialGlobalVars {
    RwInt32 engineOffset;
} RpUVAnimMaterialGlobalVars;
RW_LAYOUT_ASSERT(rw_assert_uvanim_refcount, RW_OFFSETOF(rpUVAnimCustomData, refCount) == 0x40);
RW_LAYOUT_ASSERT(rw_assert_uvanim_customdata, RW_OFFSETOF(RtAnimAnimationHead, customData) == 0x14);
RW_LAYOUT_ASSERT(rw_assert_uvanim_interp, RW_OFFSETOF(RpUVAnimMaterialExtension, interp) == 8 && sizeof(RpUVAnimMaterialExtension) == 0x28);
RW_LAYOUT_ASSERT(rw_assert_anim_interp, RW_OFFSETOF(RtAnimInterpolatorHead, currentInterpKeyFrameSize) == 0x24
                 && RW_OFFSETOF(RtAnimInterpolatorHead, numNodes) == 0x2c && RW_OFFSETOF(RtAnimInterpolatorHead, frames) == 0x4c);

/*007d2128*/ extern void *RwEngineInstance;
/*00947b58*/ extern RpUVAnimMaterialGlobalVars RpUVAnimMaterialGlobals;
/*007da8b0*/ extern void *_rpUVAnimCustomDataFreeList;
/*004d2f20*/ extern RwInt32 RwEngineRegisterPlugin(RwInt32 size, RwUInt32 pluginID, void *initCB, void *termCB);
/*004f8260*/ extern RwInt32 RpMaterialRegisterPlugin(RwInt32 size, RwUInt32 pluginID, void *constructCB, void *destructCB, void *copyCB);
/*004f8290*/ extern RwInt32 RpMaterialRegisterPluginStream(RwUInt32 pluginID, void *readCB, void *writeCB, void *getSizeCB);
/*004dc6c0*/ extern RwStream *_rwStreamWriteVersionedChunkHeader(RwStream *stream, RwInt32 type, RwInt32 size, RwUInt32 version, RwUInt32 buildNum);
/*004dc8b0*/ extern RwStream *RwStreamWriteInt32(RwStream *stream, const void *ints, RwUInt32 numBytes);
/*004dbfc0*/ extern RwStream *RwStreamWrite(RwStream *stream, const void *buffer, RwUInt32 length);
/*0053e310*/ extern RwInt32 RtAnimAnimationGetNumNodes(const void *animation);
/*0053e340*/ extern void *RtAnimInterpolatorCreate(RwInt32 numNodes, RwInt32 maxInterpKeyFrameSize);
/*0053e3b0*/ extern RwBool RtAnimInterpolatorDestroy(void *anim);
/*00573f50*/ extern RwBool RtAnimAnimationDestroy(void *animation);
/*00561be0*/ extern void *UVAnimOpen(void *instance, RwInt32 offset, RwInt32 size);
/*00561c50*/ extern void *UVAnimClose(void *instance, RwInt32 offset, RwInt32 size);
/*00561c90*/ extern void *UVAnimConstructor(void *object, RwInt32 offset, RwInt32 size);
/*00561cb0*/ extern void *UVAnimDestructor(void *object, RwInt32 offset, RwInt32 size);
/*00561ec0*/ extern RwStream *UVAnimRead(RwStream *stream, RwInt32 binaryLength, void *object, RwInt32 offsetInObject, RwInt32 sizeInObject);
RwBool RtAnimInterpolatorCopy(RtAnimInterpolatorHead *outAnim, RtAnimInterpolatorHead *inAnim);
void *UVAnimCopy(void *dstObject, const void *srcObject, RwInt32 offset, RwInt32 size);
RwStream *UVAnimWrite(RwStream *stream, RwInt32 binaryLength, const void *object, RwInt32 offsetInObject, RwInt32 sizeInObject);
RwInt32 UVAnimSize(const void *object, RwInt32 offsetInObject, RwInt32 sizeInObject);

typedef void (*RwFreeListFreeFn)(void *freeList, void *pData);
#define RW_FREELISTFREE_SLOT 0x11c      /* RwEngineInstance->memoryFuncs.rwfreelistfree (RwFreeListFree macro) */
#define RwFreeListFree(fl, p) (*(RwFreeListFreeFn *)((RwUInt8 *)RwEngineInstance + RW_FREELISTFREE_SLOT))((fl), (p))
#define RwStreamWriteChunkHeader(stream, type, size) _rwStreamWriteVersionedChunkHeader((stream), (type), (size), rwLIBRARYCURRENTVERSION, rwLIBRARYBUILDNUM)
#define RPUVANIMMATERIALGETDATA(material) ((RpUVAnimMaterialExtension *)((RwUInt8 *)(material) + RpUVAnimMaterialGlobals.engineOffset))
#define RPUVANIMMATERIALGETCONSTDATA(material) ((const RpUVAnimMaterialExtension *)((const RwUInt8 *)(material) + RpUVAnimMaterialGlobals.engineOffset))

/* TIER2: unvalidated 2026-11-09 authority=diffemu */
RwBool RtAnimInterpolatorCopy(RtAnimInterpolatorHead *outAnim, RtAnimInterpolatorHead *inAnim)
{
    memcpy(outAnim->frames, inAnim->frames, outAnim->currentInterpKeyFrameSize * outAnim->numNodes);
    return TRUE;
}

/* TIER2: unvalidated 2026-11-09 authority=diffemu */
void *UVAnimCopy(void *dstObject, const void *srcObject, RwInt32 offset, RwInt32 size)
{
    const RpUVAnimMaterialExtension *srcMaterialExt = RPUVANIMMATERIALGETCONSTDATA(srcObject);
    RpUVAnimMaterialExtension *dstMaterialExt = RPUVANIMMATERIALGETDATA(dstObject);
    RwUInt32 i;

    for (i = 0; i < RP_UVANIM_MAXSLOTS; ++i)
    {
        RtAnimInterpolatorHead *srcInterp = srcMaterialExt->interp[i];

        if (srcInterp)
        {
            RtAnimAnimationHead *anim = srcInterp->pCurrentAnim;

            dstMaterialExt->interp[i] = RtAnimInterpolatorCreate(RtAnimAnimationGetNumNodes(anim), anim->interpInfo->interpKeyFrameSize);
            if (!dstMaterialExt->interp[i])
            {
                /* release the slots copied so far (RpUVAnimDestroy + RtAnimInterpolatorDestroy) */
                RwUInt32 j;

                for (j = 0; j < i; ++j)
                {
                    RtAnimInterpolatorHead *interp = dstMaterialExt->interp[j];

                    if (interp)
                    {
                        RtAnimAnimationHead *done = interp->pCurrentAnim;

                        if (--done->customData->refCount == 0)
                        {
                            RwFreeListFree(_rpUVAnimCustomDataFreeList, done->customData);
                            RtAnimAnimationDestroy(done);
                        }
                        RtAnimInterpolatorDestroy(dstMaterialExt->interp[j]);
                    }
                }
                return NULL;
            }
            RtAnimInterpolatorCopy(dstMaterialExt->interp[i], srcInterp);

            /* RpUVAnimAddRef */
            anim->customData->refCount++;
        }
        else
        {
            dstMaterialExt->interp[i] = NULL;
        }
    }
    return dstObject;
}

/* TIER2: unvalidated 2026-11-09 authority=diffemu */
RwStream *UVAnimWrite(RwStream *stream, RwInt32 binaryLength, const void *object, RwInt32 offsetInObject, RwInt32 sizeInObject)
{
    const RpUVAnimMaterialExtension *materialExt = RPUVANIMMATERIALGETCONSTDATA(object);
    RwUInt32 size;
    RwUInt32 flags = 0;
    RwUInt32 i;

    size = sizeof(RwUInt32);

    /* Size of individual references */
    for (i = 0; i < RP_UVANIM_MAXSLOTS; ++i)
    {
        if (materialExt->interp[i])
        {
            flags |= (1 << i);
            size += RP_UVANIM_MAXNAME;
        }
    }

    if (!RwStreamWriteChunkHeader(stream, rwID_STRUCT, size))
    {
        return NULL;
    }

    /* Write out flags */
    if (!RwStreamWriteInt32(stream, &flags, sizeof(RwUInt32)))
    {
        return NULL;
    }

    /* Write individual references: the animation name (RpUVAnimGetName) */
    for (i = 0; i < RP_UVANIM_MAXSLOTS; ++i)
    {
        RtAnimInterpolatorHead *interp = materialExt->interp[i];

        if (interp)
        {
            RwStreamWrite(stream, interp->pCurrentAnim->customData->name, RP_UVANIM_MAXNAME);
        }
    }
    return stream;
}

/* TIER2: unvalidated 2026-11-09 authority=diffemu */
RwInt32 UVAnimSize(const void *object, RwInt32 offsetInObject, RwInt32 sizeInObject)
{
    const RpUVAnimMaterialExtension *matExt = RPUVANIMMATERIALGETCONSTDATA(object);
    RwUInt32 i, j;

    /* RpMaterialUVAnimExists: the material needs extension data only if it has an interpolator */
    for (i = 0; i < RP_UVANIM_MAXSLOTS; ++i)
    {
        if (matExt->interp[i])
        {
            RwInt32 size = rwCHUNKHEADERSIZE + sizeof(RwUInt32);      /* struct wrapper and flags */

            for (j = 0; j < RP_UVANIM_MAXSLOTS; ++j)
            {
                if (matExt->interp[j])
                {
                    size += RP_UVANIM_MAXNAME;
                }
            }
            return size;
        }
    }

    /* By default, do not need to stream extension data */
    return 0;
}

/* TIER2: unvalidated 2026-11-09 authority=diffemu */
RwBool RpUVAnimPluginAttach(void)
{
    RwInt32 success;

    /* Register the plugIn */
    if (RwEngineRegisterPlugin(0, rwID_UVANIMPLUGIN, (void *)UVAnimOpen, (void *)UVAnimClose) < 0)
    {
        return FALSE;
    }

    /* Extend RpMaterial to hold uvanim ptrs */
    RpUVAnimMaterialGlobals.engineOffset = RpMaterialRegisterPlugin(sizeof(RpUVAnimMaterialExtension), rwID_UVANIMPLUGIN, (void *)UVAnimConstructor,
                                                                    (void *)UVAnimDestructor, (void *)UVAnimCopy);

    /* Attach the stream handling functions */
    success = RpMaterialRegisterPluginStream(rwID_UVANIMPLUGIN, (void *)UVAnimRead, (void *)UVAnimWrite, (void *)UVAnimSize);

    return (success >= 0) && (RpUVAnimMaterialGlobals.engineOffset >= 0);
}
