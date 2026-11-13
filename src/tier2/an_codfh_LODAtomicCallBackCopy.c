/* LODAtomicCallBackCopy (MFL.exe 00568280). Adapted from codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32, src/rw/plugin/lodatm/rplodatm.c @e578672,
 * not byte-identical, changes: cut down to this function and the private declarations it uses, the static helpers
 * LODAtomicNewCache / LODAtomicInitCache / LODAtomicCopyCache inlined by hand (MFL.exe has them inlined), release build macros expanded (RWFUNCTION /
 * RWASSERT dropped, RWRETURN -> return), conditional compilation resolved for the release D3D9 build, the SDK includes replaced by rw37_slim.h,
 * data and functions MFL.exe names declared extern at their addresses. Generated, then hand-checked; compared with MFL.exe by differential
 * emulation (tier 2). */
#include "rw37_slim.h"
/* ---- SDK declarations only this row uses (generated together with rw37_slim.h: do not edit between the markers) ---- */
extern RpGeometry *
RpGeometryAddRef(RpGeometry *geometry);
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
RW_LAYOUT_ASSERT(rw37_own_0, RW_OFFSETOF(struct _rpLODAtomicCache, atomicSelectLOD) == 8 && RW_OFFSETOF(struct _rpLODAtomicCache, curLOD) == 0x14 && RW_OFFSETOF(struct _rpLODAtomicCache, lodGeometry) == 0x18);      /* MFL.exe 0x5682c3..0x5682ec (LODAtomicCallBackCopy, inlined LODAtomicInitCache): farRange / invFarRange +0xc / +0x10, atomicSelectLOD +8, curLOD +0x14, lodGeometry[10] from +0x18 */
RW_LAYOUT_ASSERT(rw37_own_1, RW_OFFSETOF(struct _rpLODAtomicGlobalVars, atmCacheFreeList) == 0xc);      /* MFL.exe 0x5682a2: atmCacheFreeList at 0x9488dc; RpLODAtomicGlobals is at 0x9488d0 */
/* TIER2: unvalidated authority=diffemu */
void * LODAtomicCallBackCopy(RpAtomic * dstAtomic, RpAtomic * srcAtomic, RwInt32 offset, RwInt32 __RWUNUSED__ size)
{
    RpLODAtomicCache   *rpSrcLODAtomicCache, *rpDstLODAtomicCache;

    rpSrcLODAtomicCache = RPLODOFFSET(srcAtomic, offset);

    if (rpSrcLODAtomicCache != NULL)
    {
        RwInt32             i;

        /* the static helpers LODAtomicNewCache, LODAtomicInitCache and LODAtomicCopyCache, inlined as in MFL.exe */
        rpDstLODAtomicCache = RPLODOFFSET(dstAtomic, offset);

        if (rpDstLODAtomicCache == NULL)
        {
            rpDstLODAtomicCache = (RpLODAtomicCache *)
                RwFreeListAlloc(RpLODAtomicGlobals.atmCacheFreeList,
                                rwID_LODATMPLUGIN | rwMEMHINTDUR_EVENT);

            if (rpDstLODAtomicCache != NULL)
            {
                rpDstLODAtomicCache->hook = FALSE;
                rpDstLODAtomicCache->atomicRender = NULL;
                rpDstLODAtomicCache->atomicSelectLOD = LODAtomicCallBackSelectLOD;

                rpDstLODAtomicCache->farRange = -1;
                rpDstLODAtomicCache->invFarRange = -1;

                rpDstLODAtomicCache->curLOD = -1;

                for (i = 0; i < RPLODATOMICMAXLOD; ++i)
                    rpDstLODAtomicCache->lodGeometry[i] = NULL;
            }
        }

        if (rpDstLODAtomicCache != NULL)
        {
            rpDstLODAtomicCache->hook = rpSrcLODAtomicCache->hook;
            rpDstLODAtomicCache->atomicRender =
                rpSrcLODAtomicCache->atomicRender;

            rpDstLODAtomicCache->atomicSelectLOD =
                rpSrcLODAtomicCache->atomicSelectLOD;

            rpDstLODAtomicCache->farRange = rpSrcLODAtomicCache->farRange;

            rpDstLODAtomicCache->invFarRange = rpSrcLODAtomicCache->invFarRange;

            rpDstLODAtomicCache->curLOD = rpSrcLODAtomicCache->curLOD;

            for (i = 0; i < RPLODATOMICMAXLOD; ++i)
            {
                rpDstLODAtomicCache->lodGeometry[i] =
                    rpSrcLODAtomicCache->lodGeometry[i];

                if (rpDstLODAtomicCache->lodGeometry[i])
                    RpGeometryAddRef(rpDstLODAtomicCache->lodGeometry[i]);
            }
        }

        RPLODOFFSET(dstAtomic, offset) = rpDstLODAtomicCache;
    }

    return (srcAtomic);
}
