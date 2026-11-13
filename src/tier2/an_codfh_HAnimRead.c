/* HAnimRead (MFL.exe 00544050). Adapted from codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32, src/rw/plugin/hanim/rphanim.c @e578672
 *, not byte-identical, changes: cut down to this function and the private declarations it uses, the
 * alignment cast through mfl_uptr, release build macros expanded (RWFUNCTION / RWASSERT dropped, RWRETURN -> return), conditional
 * compilation resolved for the release D3D9 build, the SDK includes replaced by rw37_slim.h, data and functions MFL.exe names declared
 * extern at their addresses. Generated, then hand-checked; compared with MFL.exe by differential emulation (tier 2). */
#include "rw37_slim.h"
/* ---- SDK declarations only this row uses (generated together with rw37_slim.h: do not edit between the markers) ---- */
void *__cdecl memset(void *, int, size_t);
#define RwStreamReadInt(_stream,_ints,_numBytes) RwStreamReadInt32(_stream, _ints, _numBytes)
extern RwStream *RwStreamReadInt32(RwStream *stream, RwInt32 *ints,
                                   RwUInt32 numBytes);
#define rwID_HANIMPLUGIN MAKECHUNKID(rwVENDORID_CRITERIONTK, 0x1e)
extern RtAnimInterpolator *
RtAnimInterpolatorCreate(RwInt32 numNodes,
                                RwInt32 maxInterpKeyFrameSize);
typedef struct RpHAnimFrameExtension RpHAnimFrameExtension;
struct RpHAnimFrameExtension
{
    RwInt32 id;
    RpHAnimHierarchy *hierarchy;
};
RW_LAYOUT_ASSERT(rw37_row_0, sizeof(RpHAnimFrameExtension) == 8 && RW_OFFSETOF(RpHAnimFrameExtension, hierarchy) == 4);      /* MFL.exe 0x543c81 push 0x8 (RwFrameRegisterPlugin size, RpHAnimPluginAttach); 0x543e9a mov esi,[eax+0x4] (HAnimCopy) */
/* ---- end of the generated declarations ---- */

#define RPHANIMFRAMEGETDATA(frame) ( (void *)(((RwUInt8 *)frame) + RpHAnimAtomicGlobals.engineOffset) )
#define ROUNDUP16(x) (((mfl_uptr)(x) + 16 - 1) & ~(mfl_uptr)(16 - 1))
extern RpHAnimAtomicGlobalVars RpHAnimAtomicGlobals;
/* TIER2: unvalidated authority=diffemu */
RwStream * HAnimRead(RwStream * stream, RwInt32 __RWUNUSED__ binaryLength, void *object, RwInt32 __RWUNUSED__ offsetInObject, RwInt32 __RWUNUSED__ sizeInObject)
{
    RwInt32             i;
    RwInt32             numNodes;
    RwInt32             streamVersion;
    RpHAnimNodeInfo     *pNodeInfo;
    RpHAnimHierarchy      *pHierarchy;
    RpHAnimFrameExtension *frameExt = (RpHAnimFrameExtension *)RPHANIMFRAMEGETDATA(object);

    if (!RwStreamReadInt
        (stream, (RwInt32 *) &streamVersion, sizeof(RwInt32)))
    {
        return ((RwStream *)NULL);
    }

    if (streamVersion != 0x100)
    {
        return ((RwStream *)NULL);
    }

    if (!RwStreamReadInt
        (stream, (RwInt32 *) &frameExt->id, sizeof(RwInt32)))
    {
        return ((RwStream *)NULL);
    }

    if (!RwStreamReadInt
        (stream, (RwInt32 *) &numNodes, sizeof(RwInt32)))
    {
        return ((RwStream *)NULL);
    }

    if (numNodes > 0)
    {
        RpHAnimHierarchyFlag flags;
        RwInt32 maxInterpKeyFrameSize;

        if (!RwStreamReadInt
            (stream, (RwInt32 *) &flags, sizeof(RwInt32)))
        {
            return ((RwStream *)NULL);
        }

        if (!RwStreamReadInt
            (stream, (RwInt32 *) &maxInterpKeyFrameSize, sizeof(RwInt32)))
        {
            return ((RwStream *)NULL);
        }

        pHierarchy = (RpHAnimHierarchy *)
                RwFreeListAlloc(RpHAnimAtomicGlobals.HAnimFreeList,
                    rwID_HANIMPLUGIN | rwMEMHINTDUR_EVENT);

        memset(pHierarchy, 0, sizeof(RpHAnimHierarchy));

        if (pHierarchy)
        {
            void *ptr = NULL;

            pHierarchy->currentAnim = RtAnimInterpolatorCreate(numNodes,maxInterpKeyFrameSize);

            pHierarchy->flags = flags;
            pHierarchy->parentFrame = (RwFrame *)object;
            
            pHierarchy->numNodes = numNodes;

            pHierarchy->parentHierarchy = pHierarchy;

            if (pHierarchy->flags & rpHANIMHIERARCHYNOMATRICES)
            {
                pHierarchy->pMatrixArray = (RwMatrix *) NULL;
                pHierarchy->pMatrixArrayUnaligned = NULL;
            }
            else
            {
                ptr = RwMalloc(sizeof(RwMatrix) * numNodes + 15,
                    rwID_HANIMPLUGIN | rwMEMHINTDUR_EVENT);
                pHierarchy->pMatrixArray = (RwMatrix *) ROUNDUP16(ptr);
                pHierarchy->pMatrixArrayUnaligned = ptr;
            }

            pHierarchy->pNodeInfo = (RpHAnimNodeInfo *) RwMalloc(sizeof(RpHAnimNodeInfo) * numNodes,
                                                         rwID_HANIMPLUGIN | rwMEMHINTDUR_EVENT);

            pNodeInfo = pHierarchy->pNodeInfo;

            for (i = 0; i < pHierarchy->numNodes; ++i)
            {
                if (!RwStreamReadInt
                    (stream, (RwInt32 *) & pNodeInfo->nodeID,
                     sizeof(RwInt32)))
                {
                    return ((RwStream *)NULL);
                }

                if (!RwStreamReadInt
                    (stream, (RwInt32 *) & pNodeInfo->nodeIndex,
                     sizeof(RwInt32)))
                {
                    return ((RwStream *)NULL);
                }

                if (!RwStreamReadInt
                    (stream, (RwInt32 *) & pNodeInfo->flags,
                     sizeof(RwInt32)))
                {
                    return ((RwStream *)NULL);
                }

                pNodeInfo->pFrame = (RwFrame *)NULL;

                ++pNodeInfo;
            }

        }
        frameExt->hierarchy = pHierarchy;
    }

    return (stream);
}
