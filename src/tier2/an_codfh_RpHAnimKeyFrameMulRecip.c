/* RpHAnimKeyFrameMulRecip (MFL.exe 00543a20). Adapted from codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32, src/rw/plugin/hanim/stdkey.c @e578672,
 * not byte-identical, changes: cut down to this function and the private declarations it uses, release build macros expanded (RWFUNCTION /
 * RWASSERT dropped, RWRETURN -> return), conditional compilation resolved for the release D3D9 build, the SDK includes replaced by rw37_slim.h,
 * data and functions MFL.exe names declared extern at their addresses. Generated, then hand-checked; compared with MFL.exe by differential
 * emulation (tier 2). */
#include "rw37_slim.h"
/* ---- SDK declarations only this row uses (generated together with rw37_slim.h: do not edit between the markers) ---- */
#define RwV3dIncrementScaledMacro(o,a,s) MACRO_START { (o)->x += (((a)->x) * ( (s))); (o)->y += (((a)->y) * ( (s))); (o)->z += (((a)->z) * ( (s))); } MACRO_STOP
#define RwV3dCrossProductMacro(o,a,b) MACRO_START { (o)->x = (( (((a)->y) * ( (b)->z))) - ( (((a)->z) * ( (b)->y)))); (o)->y = (( (((a)->z) * ( (b)->x))) - ( (((a)->x) * ( (b)->z)))); (o)->z = (( (((a)->x) * ( (b)->y))) - ( (((a)->y) * ( (b)->x)))); } MACRO_STOP
typedef struct RtQuat RtQuat;
struct RtQuat
{
    RwV3d imag;
    RwReal real;
};
#define RtQuatModulusSquaredMacro(q) ((q)->real * (q)->real + RwV3dDotProductMacro(&(q)->imag, &(q)->imag))
#define RtQuatMultiplyMacro(result,q1,q2) MACRO_START { (result)->real = (q1)->real * (q2)->real - RwV3dDotProductMacro(&(q1)->imag,&(q2)->imag); RwV3dCrossProductMacro(&(result)->imag, &(q1)->imag, &(q2)->imag); RwV3dIncrementScaledMacro(&(result)->imag, &(q2)->imag, (q1)->real); RwV3dIncrementScaledMacro(&(result)->imag, &(q1)->imag, (q2)->real); } MACRO_STOP
#define RtQuatReciprocalMacro(result,q) MACRO_START { RwReal val = RtQuatModulusSquaredMacro(q); if (val > (RwReal) 0) { val = ((RwReal)1) / val; (result)->real = (q)->real * val; val = -val; RwV3dScaleMacro(&(result)->imag, &(q)->imag, val); } } MACRO_STOP
#define RtQuatMultiply(result,q1,q2) RtQuatMultiplyMacro( result, q1, q2)
#define RtQuatReciprocal(result,q) RtQuatReciprocalMacro( result, q)
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
void RpHAnimKeyFrameMulRecip(void *pVoidFrame, void *pVoidStart)
{
    RpHAnimKeyFrame * pAnimFrame = (RpHAnimKeyFrame *)pVoidFrame;
    RpHAnimKeyFrame * pStartFrame = (RpHAnimKeyFrame *)pVoidStart;
    RtQuat              qInverse, qFrame;

    RtQuatReciprocal(&qInverse, &pStartFrame->q);

    qFrame = pAnimFrame->q;
    RtQuatMultiply(&pAnimFrame->q, &qInverse, &qFrame);
    RwV3dSub(&pAnimFrame->t, &pAnimFrame->t, &pStartFrame->t);

    return;

}
