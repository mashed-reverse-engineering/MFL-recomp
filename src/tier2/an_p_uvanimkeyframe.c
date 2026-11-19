/* RpUVAnimKeyFrameInit (MFL.exe 005628b0) and RpUVAnimKeyFrameStreamRead (MFL.exe 00562910): the UV animation key frame set-up and the
 * key frame stream reader of the RpUVAnim plugin (RenderWare 3.7 names, as in output/delink_sdk/lib_names.txt).  Written from the MFL.exe
 * disassembly (lane P W8); RenderWare SDK types from rw37_slim.h.  Compared with MFL.exe by differential emulation (tier 2). */
#include "rw37_slim.h"

typedef struct RpUVAnimKeyFrameHeader RpUVAnimKeyFrameHeader;
struct RpUVAnimKeyFrameHeader
{
    RpUVAnimKeyFrameHeader *prevFrame;
    RwReal                  time;
    RwUInt8                 data[0x18];      /* the interpolator's key frame data (linear: a UV matrix, param: 6 reals) */
};
RW_LAYOUT_ASSERT(p_uvk_0, sizeof(RpUVAnimKeyFrameHeader) == 0x20);      /* MFL.exe 0x562996 key frame stride 0x20; 0x56296a time + data = 0x1c bytes */

/* the interpolator infos of the two UV key frame types; typeID is their first word */
extern RwInt32 _rpUVAnimLinearInterpolatorInfo;
extern RwInt32 _rpUVAnimParamInterpolatorInfo;
#define UVANIMTYPEID(anim) (*(const RwInt32 *)((anim)->interpInfo))

extern RwBool RpUVAnimLinearKeyFrameDataInitFromMatrix(void *data, const RwMatrix *matrix);
extern RwBool RpUVAnimParamKeyFrameDataInitFromMatrix(void *data, const RwMatrix *matrix);
extern RwStream *RwStreamReadInt32(RwStream *stream, RwInt32 *ints, RwUInt32 numBytes);
extern void *_rpUVAnimCustomDataStreamRead(RwStream *stream);

/* TIER2: unvalidated authority=diffemu */
RpUVAnimKeyFrameHeader * RpUVAnimKeyFrameInit(const RtAnimAnimation *animation, RpUVAnimKeyFrameHeader *keyFrame,
                                              RpUVAnimKeyFrameHeader *prevFrame, RwReal time, const RwMatrix *matrix)
{
    keyFrame->prevFrame = prevFrame;
    keyFrame->time = time;

    if (UVANIMTYPEID(animation) == _rpUVAnimLinearInterpolatorInfo &&
        RpUVAnimLinearKeyFrameDataInitFromMatrix(keyFrame->data, matrix))
    {
        return ((RpUVAnimKeyFrameHeader *)NULL);
    }
    if (UVANIMTYPEID(animation) == _rpUVAnimParamInterpolatorInfo)
    {
        RpUVAnimParamKeyFrameDataInitFromMatrix(keyFrame->data, matrix);
    }
    return ((RpUVAnimKeyFrameHeader *)NULL);      /* MFL.exe returns 0 on every path (0x562908) */
}

/* TIER2: unvalidated 2026-11-14 authority=diffemu */
RtAnimAnimation * RpUVAnimKeyFrameStreamRead(RwStream *stream, RtAnimAnimation *animation)
{
    struct { RwInt32 prevIndex; RwInt32 version; } rec;      /* the original's two frame words, in its order */
    RpUVAnimKeyFrameHeader *frames;
    void *customData;
    RwInt32 i;

    if (!RwStreamReadInt32(stream, &rec.version, sizeof(RwInt32)))
    {
        return ((RtAnimAnimation *)NULL);
    }
    customData = _rpUVAnimCustomDataStreamRead(stream);
    if (!customData)
    {
        return ((RtAnimAnimation *)NULL);
    }

    frames = (RpUVAnimKeyFrameHeader *)animation->pFrames;
    animation->customData = customData;
    for (i = 0; i < animation->numFrames; i++)
    {
        /* time and key frame data in one RwStreamReadInt32, as MFL.exe reads them */
        if (!RwStreamReadInt32(stream, (RwInt32 *)&frames[i].time, sizeof(RwReal) + sizeof(frames[i].data)))
        {
            return ((RtAnimAnimation *)NULL);
        }
        if (!RwStreamReadInt32(stream, &rec.prevIndex, sizeof(RwInt32)))
        {
            return ((RtAnimAnimation *)NULL);
        }
        frames[i].prevFrame = &frames[rec.prevIndex];
    }
    return (animation);
}
