/* CreateDummyAnim (MFL.exe 005620a0). Adapted from codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32, src/rw/plugin/uvanim/rpuvanim.c @e578672,
 * not byte-identical, changes: cut down to this function and the private declarations it uses, the static helpers
 * RpUVAnimCreate / _rpUVAnimInit inlined by hand with this call's constant arguments (MFL.exe has them inlined), release build macros expanded (RWFUNCTION /
 * RWASSERT dropped, RWRETURN -> return), conditional compilation resolved for the release D3D9 build, the SDK includes replaced by rw37_slim.h,
 * data and functions MFL.exe names declared extern at their addresses. Generated, then hand-checked; compared with MFL.exe by differential
 * emulation (tier 2). */
#include "rw37_slim.h"
/* ---- SDK declarations only this row uses (generated together with rw37_slim.h: do not edit between the markers) ---- */
enum RwMatrixType
{
    rwMATRIXTYPENORMAL = 0x00000001,
    rwMATRIXTYPEORTHOGONAL = 0x00000002,
    rwMATRIXTYPEORTHONORMAL = 0x00000003,
    rwMATRIXTYPEMASK = 0x00000003,
    rwMATRIXTYPEFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum RwMatrixType RwMatrixType;
enum RwMatrixFlag
{
    rwMATRIXINTERNALIDENTITY = 0x00020000,
    rwMATRIXFLAGFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum RwMatrixFlag RwMatrixFlag;
#define RwMatrixSetIdentityMacro(m) MACRO_START { (m)->right.x = (m)->up.y = (m)->at.z = (RwReal)((1.0)); (m)->right.y = (m)->right.z = (m)->up.x = (RwReal)((0.0)); (m)->up.z = (m)->at.x = (m)->at.y = (RwReal)((0.0)); (m)->pos.x = (m)->pos.y = (m)->pos.z = (RwReal)((0.0)); rwMatrixSetFlags((m), rwMatrixGetFlags(m) | (rwMATRIXINTERNALIDENTITY | rwMATRIXTYPEORTHONORMAL)); } MACRO_STOP
#define rwMatrixSetFlags(m,flagsbit) ((m)->flags = (flagsbit))
#define rwMatrixGetFlags(m) ((m)->flags)
#define RwMatrixSetIdentity(m) RwMatrixSetIdentityMacro(m)
#define rwID_UVANIMPLUGIN MAKECHUNKID(rwVENDORID_CRITERIONTK, 0x35)
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
extern RtAnimAnimation *
RtAnimAnimationCreate(RwInt32 typeID,
                       RwInt32 numFrames,
                       RwInt32 flags,
                       RwReal duration);
#define RP_UVANIM_MAXSLOTS (8)
#define RP_UVANIM_MAXNAME (32)
typedef struct _rpUVAnimCustomData _rpUVAnimCustomData;
struct _rpUVAnimCustomData
{
    RwChar name[RP_UVANIM_MAXNAME];
    RwUInt32 nodeToUVChannelMap[RP_UVANIM_MAXSLOTS];
    RwUInt32 refCount;
};
typedef RtAnimAnimation RpUVAnim;
typedef struct RpUVAnimKeyFrame RpUVAnimKeyFrame;
extern RpUVAnimKeyFrame *
RpUVAnimKeyFrameInit(const RtAnimAnimation *animation,
                    RpUVAnimKeyFrame *keyFrame,
                    RpUVAnimKeyFrame *prevFrame,
                    RwReal time,
                    const RwMatrix *matrix);
RW_LAYOUT_ASSERT(rw37_row_0, sizeof(RtAnimInterpolatorInfo) == 0x30);      /* an_codfh_RtAnimAnimationStreamRead.c */
RW_LAYOUT_ASSERT(rw37_row_1, RW_OFFSETOF(RtAnimInterpolatorInfo, keyFrameStreamReadCB) == 0x20);      /* an_codfh_RtAnimAnimationStreamRead.c */
RW_LAYOUT_ASSERT(rw37_row_2, RW_OFFSETOF(RtAnimInterpolatorInfo, customDataSize) == 0x2c);      /* an_codfh_RtAnimAnimationStreamRead.c */
RW_LAYOUT_ASSERT(rw37_row_3, RW_OFFSETOF(_rpUVAnimCustomData, nodeToUVChannelMap) == 0x20);      /* an_codfh_rpUVAnimCustomDataStreamRead.c */
RW_LAYOUT_ASSERT(rw37_row_4, RW_OFFSETOF(_rpUVAnimCustomData, refCount) == 0x40);      /* an_codfh_rpUVAnimCustomDataStreamRead.c */
/* ---- end of the generated declarations ---- */

extern RwFreeList *_rpUVAnimCustomDataFreeList;
extern RtAnimInterpolatorInfo _rpUVAnimLinearInterpolatorInfo;
/* TIER2: unvalidated authority=diffemu */
RpUVAnim * CreateDummyAnim(const RwChar *name)
{
    RpUVAnim *anim;
    RwUInt32 dummyChannelMap[1] = { 0 };
    RpUVAnimKeyFrame *first, *last;
    RwMatrix identity;

    /* RpUVAnimCreate(name, 1, 2, 1.0f, dummyChannelMap, rpUVANIMLINEARKEYFRAMES) and its _rpUVAnimInit, inlined as in MFL.exe */
    anim = RtAnimAnimationCreate(_rpUVAnimLinearInterpolatorInfo.typeID, 2, 0, 1.0f);
    if (!anim)
    {
        return ( (RpUVAnim *) NULL);
    }
    {
        _rpUVAnimCustomData *customData
            = (_rpUVAnimCustomData *)RwFreeListAlloc(
                                       _rpUVAnimCustomDataFreeList,
                                       rwID_UVANIMPLUGIN | rwMEMHINTDUR_EVENT);
        if (!customData)
        {
            return ((RpUVAnim *)NULL);
        }

        strncpy(customData->name, name, RP_UVANIM_MAXNAME);
        customData->name[RP_UVANIM_MAXNAME-1]='\0';

        memcpy(customData->nodeToUVChannelMap, dummyChannelMap, sizeof(RwUInt32) * 1);

        customData->refCount = 1;

        anim->customData = customData;
    }

    first = anim->pFrames;
    last =  (RpUVAnimKeyFrame *)    
                ( ((RwUInt8 *)first)
                   + anim->interpInfo->animKeyFrameSize
                );

    RwMatrixSetIdentity(&identity);

    RpUVAnimKeyFrameInit(anim, first, (RpUVAnimKeyFrame *) NULL,
                         0.0f, &identity);

    RpUVAnimKeyFrameInit(anim, last, first,
                         1.0f, &identity);

    return (anim);
}
