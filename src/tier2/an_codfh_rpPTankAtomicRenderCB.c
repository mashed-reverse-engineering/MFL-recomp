/* rpPTankAtomicRenderCB (MFL.exe 00545ca0). Adapted from codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32, src/rw/plugin/ptank/rpptank.c @e578672,
 * not byte-identical, changes: cut down to this function and the private declarations it uses, release build macros expanded (RWFUNCTION /
 * RWASSERT dropped, RWRETURN -> return), conditional compilation resolved for the release D3D9 build, the SDK includes replaced by rw37_slim.h,
 * data and functions MFL.exe names declared extern at their addresses. Generated, then hand-checked; compared with MFL.exe by differential
 * emulation (tier 2). */
#include "rw37_slim.h"

/* TIER2: unvalidated authority=diffemu */
RpAtomic * rpPTankAtomicRenderCB(RpAtomic *atomic)
{
    RwBool callDefault = TRUE;
    RpPTankAtomicExtPrv *ptankGlobal;

    ptankGlobal = RPATOMICPTANKPLUGINDATA(atomic);

    if( 0 < ptankGlobal->actPCount )
    {
        if( ptankGlobal->ptankCallBacks.instance )
        {
            ptankGlobal->ptankCallBacks.instance(atomic, &ptankGlobal->publicData,
                            ptankGlobal->actPCount,
                            ptankGlobal->instFlags);

            ptankGlobal->instFlags = 0;
        }

        if(ptankGlobal->ptankCallBacks.render)
        {
            callDefault    = ptankGlobal->ptankCallBacks.render(
                                            atomic, &ptankGlobal->publicData,
                                            ptankGlobal->actPCount);
        }

        if( TRUE == callDefault )
        {
            ptankGlobal->defaultRenderCB(atomic);
        }

    }

    return (atomic);
}
