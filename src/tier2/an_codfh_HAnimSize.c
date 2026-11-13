/* HAnimSize (MFL.exe 00544240). Adapted from codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32, src/rw/plugin/hanim/rphanim.c @e578672,
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
RW_LAYOUT_ASSERT(rw37_row_0, sizeof(RpHAnimFrameExtension) == 8 && RW_OFFSETOF(RpHAnimFrameExtension, hierarchy) == 4);      /* MFL.exe 0x543c81 push 0x8 (RwFrameRegisterPlugin size, RpHAnimPluginAttach); 0x543e9a mov esi,[eax+0x4] (HAnimCopy) */
/* ---- end of the generated declarations ---- */

#define RPHANIMFRAMEGETCONSTDATA(frame) ( (const void *)(((const RwUInt8 *)frame) + RpHAnimAtomicGlobals.engineOffset) )
#define rpHANIMDEFAULTFRAMEID -1
extern RpHAnimAtomicGlobalVars RpHAnimAtomicGlobals;
/* TIER2: unvalidated authority=diffemu */
RwInt32 HAnimSize(const void *object, RwInt32 __RWUNUSED__ offsetInObject, RwInt32 __RWUNUSED__ sizeInObject)
{
    RwBool              needToStream;

    const RpHAnimFrameExtension *frameExt = (const RpHAnimFrameExtension *)
        RPHANIMFRAMEGETCONSTDATA(object);

    needToStream =    (rpHANIMDEFAULTFRAMEID != frameExt->id )
                   || (frameExt->hierarchy);

    if (needToStream)
    {
        RwInt32 size;

        size = sizeof(RwInt32);        
        size += sizeof(RwInt32);       
        size += sizeof(RwInt32);       

        if (frameExt->hierarchy &&
            !(frameExt->hierarchy->flags & rpHANIMHIERARCHYSUBHIERARCHY))
        {
            size += sizeof(RwInt32);   
            size += sizeof(RwInt32);   
            size +=
                frameExt->hierarchy->numNodes * (sizeof(RwInt32) * 3);
        }

        return (size);
    }

    return (0);
}
