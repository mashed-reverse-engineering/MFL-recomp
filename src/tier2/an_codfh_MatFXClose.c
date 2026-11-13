/* MatFXClose (MFL.exe 0054f1d0). Adapted from codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32, src/rw/plugin/matfx/rpmatfx.c @e578672,
 * not byte-identical, changes: cut down to this function and the private declarations it uses, release build macros expanded (RWFUNCTION /
 * RWASSERT dropped, RWRETURN -> return), conditional compilation resolved for the release D3D9 build, the SDK includes replaced by rw37_slim.h,
 * data and functions MFL.exe names declared extern at their addresses. Generated, then hand-checked; compared with MFL.exe by differential
 * emulation (tier 2). */
#include "rw37_slim.h"

typedef struct RwMatFXInfo RwMatFXInfo;
struct RwMatFXInfo
{
        RwModuleInfo Module;
        RwFreeList * MaterialData;
};
extern RwMatFXInfo MatFXInfo;
extern RwBool
_rpMatFXPipelinesDestroy(void);
extern RwMatFXInfo MatFXInfo;
RW_LAYOUT_ASSERT(rw37_own_0, RW_OFFSETOF(RwMatFXInfo, MaterialData) == 8);      /* MFL.exe 0x54f23c (MatFXOpen): the free list goes to 0x7da728; MatFXInfo is at 0x7da720, numInstances at 0x7da724 */
/* TIER2: unvalidated authority=diffemu */
void * MatFXClose(void *instance, RwInt32 offset __RWUNUSED__, RwInt32 size __RWUNUSED__)
{

    --MatFXInfo.Module.numInstances;

    if (MatFXInfo.Module.numInstances == 0)
    {
        
        _rpMatFXPipelinesDestroy();
    }

    if (MatFXInfo.MaterialData)
    {
        RwFreeListDestroy(MatFXInfo.MaterialData);
        MatFXInfo.MaterialData = (RwFreeList *)NULL;
    }

    return (instance);
}
