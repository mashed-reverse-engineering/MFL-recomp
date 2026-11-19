/* HAnimOpen (MFL.exe 00543cc0). Adapted from codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32, src/rw/plugin/hanim/rphanim.c @e578672,
 * not byte-identical, changes: cut down to this function and the private declarations it uses, release build macros expanded (RWFUNCTION /
 * RWASSERT dropped, RWRETURN -> return), conditional compilation resolved for the release D3D9 build, the SDK includes replaced by rw37_slim.h,
 * data and functions MFL.exe names declared extern at their addresses. Generated, then hand-checked; compared with MFL.exe by differential
 * emulation (tier 2). */
#include "rw37_slim.h"
/* ---- SDK declarations only this row uses (generated together with rw37_slim.h: do not edit between the markers) ---- */
#define rwID_HANIMPLUGIN MAKECHUNKID(rwVENDORID_CRITERIONTK, 0x1e)
typedef struct RtQuat RtQuat;
struct RtQuat
{
    RwV3d imag;
    RwReal real;
};
typedef void (*RtAnimKeyFrameMulRecipCallBack)
                                            (void *voidFrame, void *voidStart);
typedef RtAnimAnimation * (*RtAnimKeyFrameStreamReadCallBack)
                                (RwStream *stream, RtAnimAnimation *animation);
typedef RwBool (*RtAnimKeyFrameStreamWriteCallBack)
                       (const RtAnimAnimation *animation, RwStream *stream);
typedef RwInt32 (*RtAnimKeyFrameStreamGetSizeCallBack)
                                        (const RtAnimAnimation *animation);
struct RtAnimInterpolatorInfo
{
    RwInt32 typeID;

    RwInt32 interpKeyFrameSize;

    RwInt32 animKeyFrameSize;

    RtAnimKeyFrameApplyCallBack keyFrameApplyCB;

    RtAnimKeyFrameBlendCallBack keyFrameBlendCB;

    RtAnimKeyFrameInterpolateCallBack keyFrameInterpolateCB;

    RtAnimKeyFrameAddCallBack keyFrameAddCB;

    RtAnimKeyFrameMulRecipCallBack keyFrameMulRecipCB;

    RtAnimKeyFrameStreamReadCallBack keyFrameStreamReadCB;

    RtAnimKeyFrameStreamWriteCallBack keyFrameStreamWriteCB;

    RtAnimKeyFrameStreamGetSizeCallBack keyFrameStreamGetSizeCB;

    RwInt32 customDataSize;

};
extern RwBool
RtAnimRegisterInterpolationScheme(RtAnimInterpolatorInfo *interpolatorInfo);
#define rpHANIMSTDKEYFRAMETYPEID 0x1
typedef struct RpHAnimKeyFrame RpHAnimKeyFrame;
struct RpHAnimKeyFrame
{
    RpHAnimKeyFrame *prevFrame;
    RwReal time;
    RtQuat q;
    RwV3d t;
};
extern void
RpHAnimKeyFrameApply(void *matrix,
                     void *voidIFrame);
extern void
RpHAnimKeyFrameBlend(void *voidOut,
                        void *voidIn1,
                        void *voidIn2,
                        RwReal alpha);
extern void
RpHAnimKeyFrameInterpolate(void *voidOut,
                           void *voidIn1,
                           void *voidIn2,
                           RwReal time,
                           void *customData);
extern void
RpHAnimKeyFrameAdd(void *voidOut,
                      void *voidIn1,
                      void *voidIn2);
extern void
RpHAnimKeyFrameMulRecip(void *voidFrame,
                           void *voidStart);
extern RtAnimAnimation *
RpHAnimKeyFrameStreamRead(RwStream *stream,
                             RtAnimAnimation *animation);
extern RwBool
RpHAnimKeyFrameStreamWrite(const RtAnimAnimation *animation,
                              RwStream *stream);
extern RwInt32
RpHAnimKeyFrameStreamGetSize(const RtAnimAnimation *animation);
RW_LAYOUT_ASSERT(rw37_row_0, sizeof(RtAnimInterpolatorInfo) == 0x30);      /* an_codfh_RtAnimAnimationStreamRead.c */
RW_LAYOUT_ASSERT(rw37_row_1, RW_OFFSETOF(RtAnimInterpolatorInfo, keyFrameStreamReadCB) == 0x20);      /* an_codfh_RtAnimAnimationStreamRead.c */
RW_LAYOUT_ASSERT(rw37_row_2, RW_OFFSETOF(RtAnimInterpolatorInfo, customDataSize) == 0x2c);      /* an_codfh_RtAnimAnimationStreamRead.c */
RW_LAYOUT_ASSERT(rw37_row_3, sizeof(RpHAnimKeyFrame) == 0x24 && RW_OFFSETOF(RpHAnimKeyFrame, time) == 4 && RW_OFFSETOF(RpHAnimKeyFrame, t) == 0x18);      /* MFL.exe 0x5439f3 add esi,0x24 / 0x5439bc lea eax,[esi+0x4] (RpHAnimKeyFrameStreamWrite: frame stride, &time); 0x543d05 interpKeyFrameSize = 0x24 (HAnimOpen); t = the last 12 bytes */
RW_LAYOUT_ASSERT(rw37_row_4, RW_OFFSETOF(RpHAnimKeyFrame, q) == 8 && sizeof(RtQuat) == 0x10 && RW_OFFSETOF(RtQuat, real) == 0xc);      /* MFL.exe 0x543a27..0x543a30 / 0x543a68 (RpHAnimKeyFrameMulRecip): q.imag at +8..+0x10, q.real at +0x14 of the start frame */
/* ---- end of the generated declarations ---- */

extern RpHAnimAtomicGlobalVars RpHAnimAtomicGlobals;
extern RwInt32 _rpHAnimHierarchyFreeListBlockSize;
extern RwInt32 _rpHAnimHierarchyFreeListPreallocBlocks;
extern RwFreeList _rpHAnimHierarchyFreeList;
/* TIER2: unvalidated 2026-11-14 authority=diffemu */
void * HAnimOpen(void *instance, RwInt32 __RWUNUSED__ offset, RwInt32 __RWUNUSED__ size)
{
    RtAnimInterpolatorInfo     interpInfo;

    {
        RpHAnimAtomicGlobals.HAnimFreeList =
            RwFreeListCreateAndPreallocateSpace(sizeof(RpHAnimHierarchy),
            _rpHAnimHierarchyFreeListBlockSize, sizeof(RwInt32),
            _rpHAnimHierarchyFreeListPreallocBlocks, &_rpHAnimHierarchyFreeList,
            rwID_HANIMPLUGIN | rwMEMHINTDUR_GLOBAL);

        if (NULL == RpHAnimAtomicGlobals.HAnimFreeList)
        {
            instance = NULL;
        }
    }

    interpInfo.typeID = rpHANIMSTDKEYFRAMETYPEID;
    interpInfo.interpKeyFrameSize = sizeof(RpHAnimKeyFrame);
    interpInfo.animKeyFrameSize = sizeof(RpHAnimKeyFrame);
    interpInfo.keyFrameApplyCB = RpHAnimKeyFrameApply;
    interpInfo.keyFrameBlendCB = RpHAnimKeyFrameBlend;
    interpInfo.keyFrameInterpolateCB = RpHAnimKeyFrameInterpolate;
    interpInfo.keyFrameAddCB = RpHAnimKeyFrameAdd;
    interpInfo.keyFrameMulRecipCB = RpHAnimKeyFrameMulRecip;
    interpInfo.keyFrameStreamReadCB = RpHAnimKeyFrameStreamRead;
    interpInfo.keyFrameStreamWriteCB = RpHAnimKeyFrameStreamWrite;
    interpInfo.keyFrameStreamGetSizeCB = RpHAnimKeyFrameStreamGetSize;
    interpInfo.customDataSize = 0;

    RtAnimRegisterInterpolationScheme(&interpInfo);

    return (instance);

}
