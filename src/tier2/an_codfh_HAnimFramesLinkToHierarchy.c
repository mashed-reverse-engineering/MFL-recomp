/* HAnimFramesLinkToHierarchy (MFL.exe 005443d0). Adapted from codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32, src/rw/plugin/hanim/rphanim.c @e578672
 *, not byte-identical, changes: cut down to this function and the private declarations it uses, the
 * static helper RpHAnimFrameGetID inlined by hand (MFL.exe has it inlined), release build macros expanded (RWFUNCTION / RWASSERT dropped,
 * RWRETURN -> return), conditional compilation resolved for the release D3D9 build, the SDK includes replaced by rw37_slim.h, data and
 * functions MFL.exe names declared extern at their addresses. Generated, then hand-checked; compared with MFL.exe by differential emulation
 * (tier 2). */
#include "rw37_slim.h"
/* ---- SDK declarations only this row uses (generated together with rw37_slim.h: do not edit between the markers) ---- */
typedef RwFrame *(*RwFrameCallBack)(RwFrame *frame, void *data);
extern RwFrame *
RwFrameForAllChildren(RwFrame * frame,
                      RwFrameCallBack callBack,
                      void *data);
extern RwFrame *
RwFrameUpdateObjects(RwFrame * frame);
typedef struct RpHAnimFrameExtension RpHAnimFrameExtension;
struct RpHAnimFrameExtension
{
    RwInt32 id;
    RpHAnimHierarchy *hierarchy;
};
RW_LAYOUT_ASSERT(rw37_row_0, sizeof(RpHAnimFrameExtension) == 8 && RW_OFFSETOF(RpHAnimFrameExtension, hierarchy) == 4);      /* MFL.exe 0x543c81 push 0x8 (RwFrameRegisterPlugin size, RpHAnimPluginAttach); 0x543e9a mov esi,[eax+0x4] (HAnimCopy) */
/* ---- end of the generated declarations ---- */

#define RPHANIMFRAMEGETDATA(frame) ( (void *)(((RwUInt8 *)frame) + RpHAnimAtomicGlobals.engineOffset) )
#define rpHANIMDEFAULTFRAMEID -1
extern RpHAnimAtomicGlobalVars RpHAnimAtomicGlobals;
/* TIER2: unvalidated authority=diffemu */
/* TIER2: unvalidated 2026-11-14 authority=static */
RwFrame * HAnimFramesLinkToHierarchy(RwFrame *rootFrame, void *pData)
{
    RpHAnimHierarchy *pHierarchy = (RpHAnimHierarchy *)pData;
    int i;

    for (i=0; i<pHierarchy->numNodes; ++i)
    {
        /* RpHAnimFrameGetID(rootFrame), static in rphanim.c (defined behind its first use), inlined in MFL.exe */
        if (((RpHAnimFrameExtension *)RPHANIMFRAMEGETDATA(rootFrame))->id == pHierarchy->pNodeInfo[i].nodeID)
        {
            pHierarchy->pNodeInfo[i].pFrame = rootFrame;
        }
    }

    RwFrameUpdateObjects(rootFrame);
    RwFrameForAllChildren(rootFrame, HAnimFramesLinkToHierarchy, pData);

    return (rootFrame);
}
