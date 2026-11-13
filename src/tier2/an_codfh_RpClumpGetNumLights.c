/* RpClumpGetNumLights (MFL.exe 004f6790). Adapted from codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32, src/rw/world/baclump.c @e578672
 *, not byte-identical, changes: cut down to this function and the private declarations it uses,
 * RpClumpForAllLights with the static callback CountLight inlined by hand (MFL.exe has them inlined), release build macros expanded
 * (RWFUNCTION / RWASSERT dropped, RWRETURN -> return), conditional compilation resolved for the release D3D9 build, the SDK includes
 * replaced by rw37_slim.h, data and functions MFL.exe names declared extern at their addresses. Generated, then hand-checked; compared with
 * MFL.exe by differential emulation (tier 2). */
#include "rw37_slim.h"
/* ---- SDK declarations only this row uses (generated together with rw37_slim.h: do not edit between the markers) ---- */
#define rwLLLinkGetNext(linkvar) ((linkvar)->next)
#define rwLinkListGetFirstLLLink(list) ((list)->link.next)
#define rwLinkListGetTerminator(list) (&((list)->link))
typedef struct RpLight RpLight;
/* ---- end of the generated declarations ---- */

extern RwInt32  _rpClumpLightExtOffset;
/* TIER2: unvalidated authority=diffemu */
RwInt32 RpClumpGetNumLights(RpClump * clump)
{
    RwInt32             nNumLights = 0;

    /* RpClumpForAllLights(clump, CountLight, &nNumLights) with the static callback CountLight, both inlined in MFL.exe: count the
     * lights of the clump's light list (the walk stops at a NULL light, as the callback loop does) */
    RwLLLink           *cur = rwLinkListGetFirstLLLink(&clump->lightList);
    RwLLLink           *end = rwLinkListGetTerminator(&clump->lightList);

    while (cur != end)
    {
        /* the light of a link: link - lightExt offset - offsetof(RpClumpLightExt, inClumpLink) */
        RpLight            *light = (RpLight *)((RwUInt8 *)cur - _rpClumpLightExtOffset - sizeof(RpClump *));

        ++nNumLights;
        if (!light)
        {
            break;
        }
        cur = rwLLLinkGetNext(cur);
    }

    return (nNumLights);
}
