/* RpHAnimKeyFrameStreamWrite (MFL.exe 005439a0). Adapted from codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32, src/rw/plugin/hanim/stdkey.c @e578672,
 * not byte-identical, changes: cut down to this function and the private declarations it uses, release build macros expanded (RWFUNCTION /
 * RWASSERT dropped, RWRETURN -> return), conditional compilation resolved for the release D3D9 build, the SDK includes replaced by rw37_slim.h,
 * data and functions MFL.exe names declared extern at their addresses. Generated, then hand-checked; compared with MFL.exe by differential
 * emulation (tier 2). */
#include "rw37_slim.h"
/* ---- SDK declarations only this row uses (generated together with rw37_slim.h: do not edit between the markers) ---- */
#define RwStreamWriteInt(_stream,_ints,_numBytes) RwStreamWriteInt32(_stream, _ints, _numBytes)
extern RwStream *RwStreamWriteReal(RwStream *stream, const RwReal *reals,
                                   RwUInt32 numBytes);
extern RwStream *RwStreamWriteInt32(RwStream *stream, const RwInt32 *ints,
                                    RwUInt32 numBytes);
typedef struct RtQuat RtQuat;
struct RtQuat
{
    RwV3d imag;
    RwReal real;
};
typedef struct RpHAnimKeyFrame RpHAnimKeyFrame;
struct RpHAnimKeyFrame
{
    RpHAnimKeyFrame *prevFrame;
    RwReal time;
    RtQuat q;
    RwV3d t;
};
RW_LAYOUT_ASSERT(rw37_row_0, sizeof(RpHAnimKeyFrame) == 0x24 && RW_OFFSETOF(RpHAnimKeyFrame, time) == 4 && RW_OFFSETOF(RpHAnimKeyFrame, t) == 0x18);      /* MFL.exe 0x5439f3 add esi,0x24 / 0x5439bc lea eax,[esi+0x4] (RpHAnimKeyFrameStreamWrite: frame stride, &time); 0x543d05 interpKeyFrameSize = 0x24 (HAnimOpen); t = the last 12 bytes */
RW_LAYOUT_ASSERT(rw37_row_1, RW_OFFSETOF(RpHAnimKeyFrame, q) == 8 && sizeof(RtQuat) == 0x10 && RW_OFFSETOF(RtQuat, real) == 0xc);      /* MFL.exe 0x543a27..0x543a30 / 0x543a68 (RpHAnimKeyFrameMulRecip): q.imag at +8..+0x10, q.real at +0x14 of the start frame */
/* ---- end of the generated declarations ---- */

/* TIER2: unvalidated authority=diffemu */
RwBool RpHAnimKeyFrameStreamWrite(const RtAnimAnimation * pAnimation, RwStream * stream)
{
    RwInt32             i;
    RwInt32             temp;
    RpHAnimKeyFrame        *pFrames;

    pFrames = (RpHAnimKeyFrame *)pAnimation->pFrames;

    for (i = 0; i < pAnimation->numFrames; ++i)
    {
        if (!RwStreamWriteReal
            (stream, (RwReal *) & (pFrames[i].time),
             sizeof(RwReal) * 8))
        {
            return (FALSE);
        }

        temp = (RwInt32) ((RwUInt8 *) pFrames[i].prevFrame - (RwUInt8 *) pFrames);

        if (!RwStreamWriteInt
            (stream, (RwInt32 *) & temp, sizeof(RwInt32)))
        {
            return (FALSE);
        }
    }

    return (TRUE);
}
