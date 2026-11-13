/* rpClumpClose (MFL.exe 004f6390). Adapted from codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32, src/rw/world/baclump.c @e578672,
 * not byte-identical, changes: cut down to this function and the private declarations it uses, release build macros expanded (RWFUNCTION /
 * RWASSERT dropped, RWRETURN -> return), conditional compilation resolved for the release D3D9 build, the SDK includes replaced by rw37_slim.h,
 * data and functions MFL.exe names declared extern at their addresses. Generated, then hand-checked; compared with MFL.exe by differential
 * emulation (tier 2). */
#include "rw37_slim.h"
/* ---- SDK declarations only this row uses (generated together with rw37_slim.h: do not edit between the markers) ---- */
typedef void (*RwFreeListCallBack) (void *pMem, void *pData);
extern RwFreeList *RwFreeListForAllUsed(RwFreeList * freelist,
                                         RwFreeListCallBack
                                         fpCallBack, void *pData);
#define CS_GLOBAL_GEOMETRY_DELLOCK() 
#define CS_ATOMIC_DELLOCK() 
/* ---- end of the generated declarations ---- */

typedef struct rpClumpGlobals rpClumpGlobals;
struct rpClumpGlobals
{
    RwFreeList         *atomicFreeList;
    RwFreeList         *clumpFreeList;
};
RW_LAYOUT_ASSERT(rw37_own_0, RW_OFFSETOF(rpClumpGlobals, clumpFreeList) == 4);      /* MFL.exe 0x4f63a2: mov edx,[eax+ecx+0x4], the first RwFreeListForAllUsed (clumps) */
#define RWCLUMPGLOBAL(var) (RWPLUGINOFFSET(rpClumpGlobals, RwEngineInstance, clumpModule.globalsOffset)->var)
extern RwModuleInfo clumpModule;
extern void ClumpTidyDestroyClump( void *object, void *data __RWUNUSED__ );
extern void ClumpTidyDestroyAtomic( void *object, void *data __RWUNUSED__ );
/* TIER2: unvalidated authority=diffemu */
void * rpClumpClose(void *instance, RwInt32 offset __RWUNUSED__, RwInt32 size __RWUNUSED__)
{

    RwFreeListForAllUsed(RWCLUMPGLOBAL(clumpFreeList),
                         ClumpTidyDestroyClump, NULL);
    RwFreeListForAllUsed(RWCLUMPGLOBAL(atomicFreeList),
                         ClumpTidyDestroyAtomic, NULL);

    RwFreeListDestroy(RWCLUMPGLOBAL(atomicFreeList));
    RwFreeListDestroy(RWCLUMPGLOBAL(clumpFreeList));
    RWCLUMPGLOBAL(atomicFreeList) = (RwFreeList *)NULL;
    RWCLUMPGLOBAL(clumpFreeList) = (RwFreeList *)NULL;

    --clumpModule.numInstances;

	CS_ATOMIC_DELLOCK();

	CS_GLOBAL_GEOMETRY_DELLOCK();

    return (instance);
}
