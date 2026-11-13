/* HAnimCopy (MFL.exe 00543e80). Adapted from codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32, src/rw/plugin/hanim/rphanim.c @e578672,
 * not byte-identical, changes: cut down to this function and the private declarations it uses, release build macros expanded (RWFUNCTION /
 * RWASSERT dropped, RWRETURN -> return), conditional compilation resolved for the release D3D9 build, the SDK includes replaced by rw37_slim.h,
 * data and functions MFL.exe names declared extern at their addresses. Generated, then hand-checked; compared with MFL.exe by differential
 * emulation (tier 2). */
#include "rw37_slim.h"
/* ---- SDK declarations only this row uses (generated together with rw37_slim.h: do not edit between the markers) ---- */
typedef struct RpHAnimFrameExtension RpHAnimFrameExtension;
struct RpHAnimFrameExtension
{
    RwInt32 id;
    RpHAnimHierarchy *hierarchy;
};
extern RpHAnimHierarchy *
RpHAnimHierarchyCreate(RwInt32 numNodes,
                       RwUInt32 *nodeFlags,
                       RwInt32 *nodeIDs,
                       RpHAnimHierarchyFlag flags,
                       RwInt32 maxInterpKeyFrameSize);
RW_LAYOUT_ASSERT(rw37_row_0, sizeof(RpHAnimFrameExtension) == 8 && RW_OFFSETOF(RpHAnimFrameExtension, hierarchy) == 4);      /* MFL.exe 0x543c81 push 0x8 (RwFrameRegisterPlugin size, RpHAnimPluginAttach); 0x543e9a mov esi,[eax+0x4] (HAnimCopy) */
/* ---- end of the generated declarations ---- */

#define RPHANIMFRAMEGETDATA(frame) ( (void *)(((RwUInt8 *)frame) + RpHAnimAtomicGlobals.engineOffset) )
#define RPHANIMFRAMEGETCONSTDATA(frame) ( (const void *)(((const RwUInt8 *)frame) + RpHAnimAtomicGlobals.engineOffset) )
extern RpHAnimAtomicGlobalVars RpHAnimAtomicGlobals;
/* TIER2: unvalidated authority=diffemu */
void * HAnimCopy(void *dstObject, const void * srcObject, RwInt32 __RWUNUSED__ offset, RwInt32 __RWUNUSED__ size)
{
    const RpHAnimFrameExtension *srcFrameExt;
    RpHAnimFrameExtension *dstFrameExt;

    srcFrameExt = (const RpHAnimFrameExtension *)
        RPHANIMFRAMEGETCONSTDATA(srcObject);
    dstFrameExt = (RpHAnimFrameExtension *)
        RPHANIMFRAMEGETDATA(dstObject);

    dstFrameExt->id = srcFrameExt->id;
    if (srcFrameExt->hierarchy)
    {
        RpHAnimHierarchy *srcHierarchy = srcFrameExt->hierarchy;
        if (!(srcHierarchy->flags & rpHANIMHIERARCHYSUBHIERARCHY))
        {
            RpHAnimHierarchy *dstHierarchy;
            RwInt32 i;

            dstHierarchy =
                RpHAnimHierarchyCreate(srcHierarchy->numNodes,
                                    (RwUInt32 *)NULL,
                                    (RwInt32 *)NULL,
                                    (RpHAnimHierarchyFlag)srcHierarchy->flags,
                                    srcHierarchy->currentAnim->maxInterpKeyFrameSize);

            for(i=0; i<dstHierarchy->numNodes; ++i)
            {
                dstHierarchy->pNodeInfo[i].pFrame = (RwFrame *)NULL;
                dstHierarchy->pNodeInfo[i].flags = srcHierarchy->pNodeInfo[i].flags;
                dstHierarchy->pNodeInfo[i].nodeIndex = srcHierarchy->pNodeInfo[i].nodeIndex;
                dstHierarchy->pNodeInfo[i].nodeID = srcHierarchy->pNodeInfo[i].nodeID;
            }

            dstFrameExt->hierarchy = dstHierarchy;
            dstHierarchy->parentFrame = (RwFrame *)dstObject;
        }
    }

    return (dstObject);
}
