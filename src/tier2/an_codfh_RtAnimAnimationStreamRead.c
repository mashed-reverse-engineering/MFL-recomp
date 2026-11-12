/* RtAnimAnimationStreamRead (MFL.exe 0x0053e040): RenderWare Graphics 3.7 (Criterion Software), reads an animation (version, type,
 * frame count, flags, duration, then the keyframes through the interpolation scheme's stream read callback) from a stream.
 * Adapted from codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32, src/rw/tool/anim/rtanim.c @e578672,
 * not byte-identical, changes: PC build (RwStreamReadInt / RwStreamReadReal are the exe's RwStreamReadInt32, the linker folded the
 * identical functions), RWAPIFUNCTION / RWASSERT / RWRETURN expanded, RWERROR expanded as the release build has it (_rwerror +
 * RwErrorSet, plugin ID rwID_RTANIM), RtAnimGetInterpolatorInfo / RtAnimAnimationCreate written out (the exe inlines them here),
 * the structs declared here (codfh rtanim.h / baerr.h) with compile-time layout asserts.
 * Verified by tools/diffemu (tier 2, chunk-stream spec s_codfh_F2.py). */
#include "mfl_tier2.h"

#define RW_OFFSETOF(type, member) ((char *)&((type *)0)->member - (char *)0)      /* pointer difference: no pointer-to-int cast */
#define RW_LAYOUT_ASSERT(name, cond) typedef char name[(sizeof(void *) != 4 || (cond)) ? 1 : -1]      /* cl 13.00 has no _Static_assert; the offsets are the 32-bit exe's */

typedef struct RtAnimAnimation RtAnimAnimation;
typedef RtAnimAnimation *(*RtAnimKeyFrameStreamReadCallBack)(RwStream *stream, RtAnimAnimation *animation);
typedef struct RtAnimInterpolatorInfo {
    RwInt32 typeID;
    RwInt32 interpKeyFrameSize;
    RwInt32 animKeyFrameSize;
    void *keyFrameApplyCB;
    void *keyFrameBlendCB;
    void *keyFrameInterpolateCB;
    void *keyFrameAddCB;
    void *keyFrameMulRecipCB;
    RtAnimKeyFrameStreamReadCallBack keyFrameStreamReadCB;
    void *keyFrameStreamWriteCB;
    void *keyFrameStreamGetSizeCB;
    RwInt32 customDataSize;
} RtAnimInterpolatorInfo;
struct RtAnimAnimation {
    RtAnimInterpolatorInfo *interpInfo;
    RwInt32 numFrames;
    RwInt32 flags;
    RwReal duration;
    void *pFrames;
    void *customData;
};
typedef struct RwError {
    RwInt32 pluginID;
    RwInt32 errorCode;
} RwError;
RW_LAYOUT_ASSERT(rw_assert_interp_size, sizeof(RtAnimInterpolatorInfo) == 0x30);
RW_LAYOUT_ASSERT(rw_assert_interp_read, RW_OFFSETOF(RtAnimInterpolatorInfo, keyFrameStreamReadCB) == 0x20);
RW_LAYOUT_ASSERT(rw_assert_interp_custom, RW_OFFSETOF(RtAnimInterpolatorInfo, customDataSize) == 0x2c);
RW_LAYOUT_ASSERT(rw_assert_anim_size, sizeof(RtAnimAnimation) == 0x18);

#define rwID_RTANIM 0x1b7
#define rwID_HANIMANIMATION 0x1b
#define rwMEMHINTDUR_EVENT 0x30000
#define E_RT_ANIM_INTERP_IDUNKNOWN 2

/*007d2128*/ extern void *RwEngineInstance;
/*007d9e18*/ extern RwInt32 RtAnimInterpolatorInfoBlockNumEntries;
/*009477c0*/ extern RtAnimInterpolatorInfo RtAnimInterpolatorInfoBlock[];
/*004dc8d0*/ extern RwStream *RwStreamReadInt32(RwStream *stream, RwInt32 *ints, RwUInt32 numBytes);
/*0053eac0*/ extern RwInt32 _rwerror(RwInt32 code, ...);
/*004ea160*/ extern RwError *RwErrorSet(RwError *code);

#define RwStreamReadReal(stream, reals, numBytes) RwStreamReadInt32((stream), (RwInt32 *)(reals), (numBytes))
#define RWERROR_SET(code) { RwError error; error.pluginID = rwID_RTANIM; error.errorCode = _rwerror(code); RwErrorSet(&error); }

/* RtAnimGetInterpolatorInfo, inlined by the exe: the info block entry of typeID, else RWERROR + NULL */
#define RtAnimGetInterpolatorInfoInline(out, typeID)                            \
    do {                                                                        \
        RwInt32 i_;                                                             \
        (out) = NULL;                                                           \
        for (i_ = 0; i_ < RtAnimInterpolatorInfoBlockNumEntries; i_++)          \
        {                                                                       \
            if (RtAnimInterpolatorInfoBlock[i_].typeID == (typeID))             \
            {                                                                   \
                (out) = &RtAnimInterpolatorInfoBlock[i_];                       \
                break;                                                          \
            }                                                                   \
        }                                                                       \
        if (!(out))                                                             \
        {                                                                       \
            RWERROR_SET(E_RT_ANIM_INTERP_IDUNKNOWN);      /* Unregistered animation type */ \
        }                                                                       \
    } while (0)

/* TIER2: validated 2026-11-10 authority=diffemu */
RtAnimAnimation *RtAnimAnimationStreamRead(RwStream *stream)
{
    RwInt32 streamVersion;
    RwInt32 typeID;
    RtAnimInterpolatorInfo *interpInfo;
    RtAnimAnimation *pAnimation;
    RwInt32 numFrames;
    RwInt32 flags;
    RwReal duration;

    if (!RwStreamReadInt32(stream, &streamVersion, sizeof(RwInt32)))
    {
        return NULL;
    }

    if (streamVersion != 0x100)
    {
        return NULL;
    }

    if (!RwStreamReadInt32(stream, &typeID, sizeof(RwInt32)))
    {
        return NULL;
    }

    RtAnimGetInterpolatorInfoInline(interpInfo, typeID);
    if (!interpInfo)
    {
        return NULL;
    }

    if (!RwStreamReadInt32(stream, &numFrames, sizeof(RwInt32)))
    {
        return NULL;
    }

    if (!RwStreamReadInt32(stream, &flags, sizeof(RwInt32)))
    {
        return NULL;
    }

    if (!RwStreamReadReal(stream, &duration, sizeof(RwReal)))
    {
        return NULL;
    }

    /* allocate the animation (RtAnimAnimationCreate) */
    {
        RtAnimInterpolatorInfo *createInfo;

        RtAnimGetInterpolatorInfoInline(createInfo, typeID);
        if (!createInfo)
        {
            /* Unregistered animation type */
            return NULL;
        }

        pAnimation = (RtAnimAnimation *)RwMalloc(sizeof(RtAnimAnimation) + numFrames * createInfo->animKeyFrameSize + createInfo->customDataSize,
                                                 rwID_HANIMANIMATION | rwMEMHINTDUR_EVENT);

        pAnimation->numFrames = numFrames;
        pAnimation->duration = duration;
        pAnimation->flags = flags;
        pAnimation->interpInfo = createInfo;
        pAnimation->pFrames = (void *)(pAnimation + 1);
        if (createInfo->customDataSize > 0)
        {
            pAnimation->customData = (void *)(((RwUInt8 *)pAnimation->pFrames) + numFrames * createInfo->animKeyFrameSize);
        }
        else
        {
            pAnimation->customData = NULL;
        }
    }

    interpInfo->keyFrameStreamReadCB(stream, pAnimation);

    return pAnimation;
}
