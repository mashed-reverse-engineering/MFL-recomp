/* LODAtomicCallBackDestructor (MFL.exe 00568360). Adapted from codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32, src/rw/plugin/lodatm/rplodatm.c @e578672,
 * not byte-identical, changes: cut down to this function and the private declarations it uses, the static helpers
 * LODAtomicUnHookRender / LODAtomicDestroyCache / LODAtomicInitCache inlined by hand (MFL.exe has them inlined), release build macros expanded (RWFUNCTION /
 * RWASSERT dropped, RWRETURN -> return), conditional compilation resolved for the release D3D9 build, the SDK includes replaced by rw37_slim.h,
 * data and functions MFL.exe names declared extern at their addresses. Generated, then hand-checked; compared with MFL.exe by differential
 * emulation (tier 2). */
#include "rw37_slim.h"
/* ---- SDK declarations only this row uses (generated together with rw37_slim.h: do not edit between the markers) ---- */
extern RwBool
RpGeometryDestroy(RpGeometry *geometry);
/* ---- end of the generated declarations ---- */

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
#define RPLODOFFSET(_ptr,_offset) (*(RpLODAtomicCache **)(((char *)(_ptr)) + _offset))
extern RpLODAtomicGlobalVars RpLODAtomicGlobals;
extern RwInt32 LODAtomicCallBackSelectLOD(RpAtomic * atomic);
RW_LAYOUT_ASSERT(rw37_own_0, RW_OFFSETOF(struct _rpLODAtomicCache, atomicSelectLOD) == 8 && RW_OFFSETOF(struct _rpLODAtomicCache, curLOD) == 0x14 && RW_OFFSETOF(struct _rpLODAtomicCache, lodGeometry) == 0x18);      /* MFL.exe 0x5683c7..0x5683e3 (LODAtomicCallBackDestructor): farRange / invFarRange +0xc / +0x10, atomicSelectLOD +8, curLOD +0x14, lodGeometry[10] from +0x18 */
RW_LAYOUT_ASSERT(rw37_own_1, RW_OFFSETOF(struct _rpLODAtomicGlobalVars, atmCacheFreeList) == 0xc);      /* MFL.exe 0x5683ed: atmCacheFreeList at 0x9488dc; RpLODAtomicGlobals is at 0x9488d0 */
/* TIER2: unvalidated authority=diffemu */
void * LODAtomicCallBackDestructor(RpAtomic * atomic, RwInt32 offset, RwInt32 __RWUNUSED__ size)
{
    RpLODAtomicCache   *rpLODAtomicCache;

    rpLODAtomicCache = RPLODOFFSET(atomic, offset);

    if (rpLODAtomicCache != NULL)
    {
        RwInt32             i;

        /* the static helpers LODAtomicUnHookRender, LODAtomicDestroyCache and LODAtomicInitCache, inlined as in MFL.exe */
        if (rpLODAtomicCache->hook)
        {
            if (rpLODAtomicCache->atomicRender != NULL)
                RpAtomicSetRenderCallBack(atomic,
                                          rpLODAtomicCache->atomicRender);

            rpLODAtomicCache->atomicRender = NULL;
            rpLODAtomicCache->hook = FALSE;
        }

        for (i = 0; i < RPLODATOMICMAXLOD; ++i)
        {
            if (rpLODAtomicCache->lodGeometry[i])
                RpGeometryDestroy(rpLODAtomicCache->lodGeometry[i]);
        }

        rpLODAtomicCache->hook = FALSE;
        rpLODAtomicCache->atomicRender = NULL;
        rpLODAtomicCache->atomicSelectLOD = LODAtomicCallBackSelectLOD;

        rpLODAtomicCache->farRange = -1;
        rpLODAtomicCache->invFarRange = -1;

        rpLODAtomicCache->curLOD = -1;

        for (i = 0; i < RPLODATOMICMAXLOD; ++i)
            rpLODAtomicCache->lodGeometry[i] = NULL;

        RwFreeListFree(RpLODAtomicGlobals.atmCacheFreeList,
                       rpLODAtomicCache);

        RPLODOFFSET(atomic, offset) = NULL;
    }

    return (atomic);
}
