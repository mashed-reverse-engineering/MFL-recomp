/* RpClumpForAllAtomics (MFL.exe 004f6810). Adapted from codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32, src/rw/world/baclump.c @e578672,
 * not byte-identical, changes: cut down to this function and the private declarations it uses, release build macros expanded (RWFUNCTION /
 * RWASSERT dropped, RWRETURN -> return), conditional compilation resolved for the release D3D9 build, the SDK includes replaced by rw37_slim.h,
 * data and functions MFL.exe names declared extern at their addresses. Generated, then hand-checked; compared with MFL.exe by differential
 * emulation (tier 2). */
#include "rw37_slim.h"
/* ---- SDK declarations only this row uses (generated together with rw37_slim.h: do not edit between the markers) ---- */
#define rwLLLinkGetData(linkvar,type,entry) ((type *)(((RwUInt8 *)(linkvar))-offsetof(type,entry)))
#define rwLLLinkGetNext(linkvar) ((linkvar)->next)
#define rwLinkListGetFirstLLLink(list) ((list)->link.next)
#define rwLinkListGetTerminator(list) (&((list)->link))
typedef RpAtomic *(*RpAtomicCallBack) (RpAtomic * atomic, void *data);
/* ---- end of the generated declarations ---- */

/* TIER2: unvalidated authority=diffemu */
RpClump * RpClumpForAllAtomics(RpClump * clump, RpAtomicCallBack callback, void *pData)
{
    RwLLLink           *cur, *end, *next;

    cur = rwLinkListGetFirstLLLink(&clump->atomicList);
    end = rwLinkListGetTerminator(&clump->atomicList);

    while (cur != end)
    {
        RpAtomic *atomic = rwLLLinkGetData(cur, RpAtomic, inClumpLink);

        next = rwLLLinkGetNext(cur);

        if (!callback(atomic, pData))
        {
            
            return (clump);
        }

        cur = next;
    }

    return (clump);
}
