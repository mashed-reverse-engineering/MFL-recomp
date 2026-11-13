/* RpHAnimHierarchyCreate (MFL.exe 00544280). Adapted from codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32, src/rw/plugin/hanim/rphanim.c @e578672,
 * not byte-identical, changes: cut down to this function and the private declarations it uses, release build macros expanded (RWFUNCTION /
 * RWASSERT dropped, RWRETURN -> return), conditional compilation resolved for the release D3D9 build, the SDK includes replaced by rw37_slim.h,
 * data and functions MFL.exe names declared extern at their addresses. Generated, then hand-checked; compared with MFL.exe by differential
 * emulation (tier 2). */
#include "rw37_slim.h"
/* ---- SDK declarations only this row uses (generated together with rw37_slim.h: do not edit between the markers) ---- */
#define rwID_HANIMPLUGIN MAKECHUNKID(rwVENDORID_CRITERIONTK, 0x1e)
extern RtAnimInterpolator *
RtAnimInterpolatorCreate(RwInt32 numNodes,
                                RwInt32 maxInterpKeyFrameSize);
/* ---- end of the generated declarations ---- */

#define ROUNDUP16(x) (((mfl_uptr)(x) + 16 - 1) & ~(mfl_uptr)(16 - 1))
extern RpHAnimAtomicGlobalVars RpHAnimAtomicGlobals;
/* TIER2: unvalidated authority=diffemu */
RpHAnimHierarchy * RpHAnimHierarchyCreate(RwInt32 numNodes, RwUInt32 *nodeFlags, RwInt32 *nodeIDs, RpHAnimHierarchyFlag flags, RwInt32 maxInterpKeyFrameSize)
{
    void               *ptr;
    RpHAnimHierarchy    *pHierarchy;
    int                 node;

    ptr = RwFreeListAlloc(RpHAnimAtomicGlobals.HAnimFreeList,
        rwID_HANIMPLUGIN | rwMEMHINTDUR_EVENT);

    pHierarchy = (RpHAnimHierarchy *) ptr;

    pHierarchy->currentAnim = RtAnimInterpolatorCreate(numNodes,maxInterpKeyFrameSize);

    pHierarchy->flags = flags;
    pHierarchy->numNodes = numNodes;
    pHierarchy->parentFrame = (RwFrame *)NULL;

    if (!(flags & rpHANIMHIERARCHYNOMATRICES))
    {
        ptr = RwMalloc(sizeof(RwMatrix) * numNodes + 15,
            rwID_HANIMPLUGIN | rwMEMHINTDUR_EVENT);
        pHierarchy->pMatrixArray = (RwMatrix *) ROUNDUP16(ptr);
        pHierarchy->pMatrixArrayUnaligned = ptr;
    }
    else
    {
        pHierarchy->pMatrixArray = NULL;
        pHierarchy->pMatrixArrayUnaligned = NULL;
    }

    pHierarchy->pNodeInfo = (RpHAnimNodeInfo *)  RwMalloc(sizeof(RpHAnimNodeInfo) * numNodes,
                                                      rwID_HANIMPLUGIN | rwMEMHINTDUR_EVENT);

    for (node = 0; node < numNodes; ++node)
    {
        pHierarchy->pNodeInfo[node].pFrame = (RwFrame *)NULL;
        if (nodeIDs)
        {
            pHierarchy->pNodeInfo[node].nodeID = nodeIDs[node];
        }
        pHierarchy->pNodeInfo[node].nodeIndex = node;
        if (nodeFlags)
        {
            pHierarchy->pNodeInfo[node].flags = nodeFlags[node];
        }
    }

    pHierarchy->parentHierarchy = pHierarchy;

    return (pHierarchy);
}
