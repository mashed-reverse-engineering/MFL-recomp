/* LODAtomicCallBackOpen (MFL.exe 00568420). Adapted from codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32, src/rw/plugin/lodatm/rplodatm.c @e578672,
 * not byte-identical, changes: cut down to this function and the private declarations it uses, release build macros expanded (RWFUNCTION /
 * RWASSERT dropped, RWRETURN -> return), conditional compilation resolved for the release D3D9 build, the SDK includes replaced by rw37_slim.h,
 * data and functions MFL.exe names declared extern at their addresses. Generated, then hand-checked; compared with MFL.exe by differential
 * emulation (tier 2). */
#include "rw37_slim.h"
/* ---- SDK declarations only this row uses (generated together with rw37_slim.h: do not edit between the markers) ---- */
typedef unsigned int undefined4, uint;
/* ---- end of the generated declarations ---- */

extern undefined4 _rpLODAtomicCacheFreeListPreallocBlocks;
#define RPLODATOMICMAXLOD 10
typedef RwInt32 (*RpLODAtomicLODCallBack)( RpAtomic *atomic );
typedef struct _rpLODAtomicGlobalVars RpLODAtomicGlobalVars;
struct _rpLODAtomicGlobalVars
{
    RwInt32             engineOffset; 
    RwInt32             atmCacheOffset; 
    RwCamera           *atmCamera;
    RwFreeList         *atmCacheFreeList;
};
typedef struct _rpLODAtomicCache RpLODAtomicCache;
struct _rpLODAtomicCache
{
    RwBool              hook;
    RpAtomicCallBackRender atomicRender;
    RpLODAtomicLODCallBack atomicSelectLOD;
    RwReal              farRange;
    RwReal              invFarRange;
    RwInt32             curLOD;
    RpGeometry         *lodGeometry[RPLODATOMICMAXLOD];
};
extern RpLODAtomicGlobalVars RpLODAtomicGlobals;
extern RwInt32 _rpLODAtomicCacheFreeListBlockSize;
extern RwFreeList _rpLODAtomicCacheFreeList;
RW_LAYOUT_ASSERT(rw37_own_0, sizeof(struct _rpLODAtomicCache) == 0x40);      /* MFL.exe 0x56843e: push 0x40, entry size of the cache free list */
RW_LAYOUT_ASSERT(rw37_own_1, RW_OFFSETOF(struct _rpLODAtomicGlobalVars, atmCacheFreeList) == 0xc);      /* MFL.exe 0x568448: mov ds:0x9488dc,eax; RpLODAtomicGlobals is at 0x9488d0 */
/* TIER2: unvalidated authority=diffemu */
void * LODAtomicCallBackOpen(void *ptr, RwInt32 __RWUNUSED__ offset, RwInt32 __RWUNUSED__ size)
{
    void               *result;

    result = ptr;

    RpLODAtomicGlobals.atmCacheFreeList =
        RwFreeListCreateAndPreallocateSpace(sizeof(RpLODAtomicCache), _rpLODAtomicCacheFreeListBlockSize,
        sizeof(RwUInt32), _rpLODAtomicCacheFreeListPreallocBlocks, &_rpLODAtomicCacheFreeList,
        rwID_LODATMPLUGIN | rwMEMHINTDUR_EVENT);

    if (RpLODAtomicGlobals.atmCacheFreeList == NULL)
        result = NULL;

    return (result);
}
