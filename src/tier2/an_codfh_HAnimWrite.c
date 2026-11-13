/* HAnimWrite (MFL.exe 00543f20). Adapted from codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32, src/rw/plugin/hanim/rphanim.c @e578672,
 * not byte-identical, changes: cut down to this function and the private declarations it uses, release build macros expanded (RWFUNCTION /
 * RWASSERT dropped, RWRETURN -> return), conditional compilation resolved for the release D3D9 build, the SDK includes replaced by rw37_slim.h,
 * data and functions MFL.exe names declared extern at their addresses. Generated, then hand-checked; compared with MFL.exe by differential
 * emulation (tier 2). */
#include "rw37_slim.h"
/* ---- SDK declarations only this row uses (generated together with rw37_slim.h: do not edit between the markers) ---- */
#define RwStreamWriteInt(_stream,_ints,_numBytes) RwStreamWriteInt32(_stream, _ints, _numBytes)
extern RwStream *RwStreamWriteInt32(RwStream *stream, const RwInt32 *ints,
                                    RwUInt32 numBytes);
#define rpHANIMSTREAMCURRENTVERSION 0x100
typedef struct RpHAnimFrameExtension RpHAnimFrameExtension;
struct RpHAnimFrameExtension
{
    RwInt32 id;
    RpHAnimHierarchy *hierarchy;
};
RW_LAYOUT_ASSERT(rw37_row_0, sizeof(RpHAnimFrameExtension) == 8 && RW_OFFSETOF(RpHAnimFrameExtension, hierarchy) == 4);      /* MFL.exe 0x543c81 push 0x8 (RwFrameRegisterPlugin size, RpHAnimPluginAttach); 0x543e9a mov esi,[eax+0x4] (HAnimCopy) */
/* ---- end of the generated declarations ---- */

#define RPHANIMFRAMEGETCONSTDATA(frame) ( (const void *)(((const RwUInt8 *)frame) + RpHAnimAtomicGlobals.engineOffset) )
extern RpHAnimAtomicGlobalVars RpHAnimAtomicGlobals;
/* TIER2: unvalidated authority=diffemu */
RwStream * HAnimWrite(RwStream * stream, RwInt32 __RWUNUSED__ binaryLength, const void *object, RwInt32 __RWUNUSED__ offsetInObject, RwInt32 __RWUNUSED__ sizeInObject)
{
    RwInt32             i;
    const RpHAnimFrameExtension *frameExt;
    RpHAnimNodeInfo     *pNodeInfo;
    RpHAnimHierarchy    *animHierarchy;
    RwInt32              streamVersion = rpHANIMSTREAMCURRENTVERSION;

    if (!RwStreamWriteInt
        (stream, (RwInt32 *) &streamVersion, sizeof(RwInt32)))
    {
        return ((RwStream *)NULL);
    }

    frameExt = (const RpHAnimFrameExtension *)RPHANIMFRAMEGETCONSTDATA(object);

    if (!RwStreamWriteInt
        (stream, (const RwInt32 *) &frameExt->id, sizeof(RwInt32)))
    {
        return ((RwStream *)NULL);
    }

    animHierarchy = frameExt->hierarchy;

    if (animHierarchy &&
        !(animHierarchy->flags & rpHANIMHIERARCHYSUBHIERARCHY))
    {
        
        if (!RwStreamWriteInt
            (stream, (const RwInt32 *) &animHierarchy->numNodes,
             sizeof(RwInt32)))
        {
            return ((RwStream *)NULL);
        }

        if (!RwStreamWriteInt
            (stream, (const RwInt32 *) &animHierarchy->flags,
             sizeof(RwInt32)))
        {
            return ((RwStream *)NULL);
        }

        if (!RwStreamWriteInt
            (stream, (const RwInt32 *) &animHierarchy->currentAnim->maxInterpKeyFrameSize,
             sizeof(RwInt32)))
        {
            return ((RwStream *)NULL);
        }

        pNodeInfo = animHierarchy->pNodeInfo;

        for (i = 0; i < animHierarchy->numNodes; ++i)
        {
            if (!RwStreamWriteInt
                (stream, (RwInt32 *) & pNodeInfo->nodeID,
                 sizeof(RwInt32)))
            {
                return ((RwStream *)NULL);
            }

            if (!RwStreamWriteInt
                (stream, (RwInt32 *) & pNodeInfo->nodeIndex,
                 sizeof(RwInt32)))
            {
                return ((RwStream *)NULL);
            }

            if (!RwStreamWriteInt
                (stream, (RwInt32 *) & pNodeInfo->flags,
                 sizeof(RwInt32)))
            {
                return ((RwStream *)NULL);
            }

            pNodeInfo++;
        }

    }
    else
    {
        RwInt32 numNodes = 0;

        if (!RwStreamWriteInt
            (stream, (const RwInt32 *) &numNodes,
             sizeof(RwInt32)))
        {
            return ((RwStream *)NULL);
        }
    }

    return (stream);
}
