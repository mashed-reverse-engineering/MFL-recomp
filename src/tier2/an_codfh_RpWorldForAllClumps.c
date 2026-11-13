/* RpWorldForAllClumps (MFL.exe 004f5d90). Adapted from codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32, src/rw/world/baworld.c @e578672,
 * not byte-identical, changes: cut down to this function and the private declarations it uses, release build macros expanded (RWFUNCTION /
 * RWASSERT dropped, RWRETURN -> return), conditional compilation resolved for the release D3D9 build, the SDK includes replaced by rw37_slim.h,
 * data and functions MFL.exe names declared extern at their addresses. Generated, then hand-checked; compared with MFL.exe by differential
 * emulation (tier 2). */
#include "rw37_slim.h"
/* ---- SDK declarations only this row uses (generated together with rw37_slim.h: do not edit between the markers) ---- */
#define rwLLLinkGetData(linkvar,type,entry) ((type *)(((RwUInt8 *)(linkvar))-offsetof(type,entry)))
#define rwLLLinkGetNext(linkvar) ((linkvar)->next)
#define rwLinkListEmpty(list) (((list)->link.next) == (&(list)->link))
#define rwLinkListGetFirstLLLink(list) ((list)->link.next)
#define rwLinkListGetTerminator(list) (&((list)->link))
#define THREADSAFE_CHECK_ISCALLEDMAIN() 
/* ---- end of the generated declarations ---- */

/* TIER2: unvalidated authority=diffemu */
RpWorld * RpWorldForAllClumps(RpWorld * world, RpClumpCallBack fpCallBack, void *pData)
{

	THREADSAFE_CHECK_ISCALLEDMAIN();

    if (!rwLinkListEmpty(&world->clumpList))
    {
        RwLLLink           *cur, *term, *next;

        cur = rwLinkListGetFirstLLLink(&world->clumpList);
        term = rwLinkListGetTerminator(&world->clumpList);

        while (term != cur)
        {
            RpClump *clump;

            clump = rwLLLinkGetData(cur, RpClump, inWorldLink);

            next = rwLLLinkGetNext(cur);
            if (!fpCallBack(clump, pData))
            {
                
                return (world);
            }

            cur = next;
        }
    }

    return (world);
}
