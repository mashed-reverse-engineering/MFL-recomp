/* WorldObjectClose (MFL.exe 004f38b0). Adapted from codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32, src/rw/world/baworobj.c @e578672,
 * not byte-identical, changes: cut down to this function and the private declarations it uses, release build macros expanded (RWFUNCTION /
 * RWASSERT dropped, RWRETURN -> return), conditional compilation resolved for the release D3D9 build, the SDK includes replaced by rw37_slim.h,
 * data and functions MFL.exe names declared extern at their addresses. Generated, then hand-checked; compared with MFL.exe by differential
 * emulation (tier 2). */
#include "rw37_slim.h"

typedef struct rpWorldObjGlobals rpWorldObjGlobals;
struct rpWorldObjGlobals
{
    RwFreeList         *tieFreeList;
    RwFreeList         *lightTieFreeList;
    RwUInt32            clumpsInFrustumID;
};
RW_LAYOUT_ASSERT(rw37_own_0, RW_OFFSETOF(rpWorldObjGlobals, lightTieFreeList) == 4);      /* MFL.exe 0x4f38bb: mov eax,[eax+ecx+0x4], destroyed first */
#define RPWORLDOBJGLOBAL(var) (RWPLUGINOFFSET(rpWorldObjGlobals, RwEngineInstance, worldObjModule.globalsOffset)->var)
extern RwModuleInfo worldObjModule;
/* TIER2: unvalidated authority=diffemu */
void * WorldObjectClose(void *instance, RwInt32 offset __RWUNUSED__, RwInt32 size __RWUNUSED__)
{

    if (RPWORLDOBJGLOBAL(lightTieFreeList))
    {
        RwFreeListDestroy(RPWORLDOBJGLOBAL(lightTieFreeList));
        RPWORLDOBJGLOBAL(lightTieFreeList) = (RwFreeList *)NULL;
    }

    if (RPWORLDOBJGLOBAL(tieFreeList))
    {
        RwFreeListDestroy(RPWORLDOBJGLOBAL(tieFreeList));
        RPWORLDOBJGLOBAL(tieFreeList) = (RwFreeList *)NULL;
    }

    worldObjModule.numInstances--;

    return (instance);
}
