/* PVSGetNumSectors (MFL.exe 00549240). Adapted from codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32, src/rw/plugin/pvs/rppvsaux.c @e578672,
 * not byte-identical, changes: cut down to this function and the private declarations it uses, release build macros expanded (RWFUNCTION /
 * RWASSERT dropped, RWRETURN -> return), conditional compilation resolved for the release D3D9 build, the SDK includes replaced by rw37_slim.h,
 * data and functions MFL.exe names declared extern at their addresses. Generated, then hand-checked; compared with MFL.exe by differential
 * emulation (tier 2). */
#include "rw37_slim.h"
/* ---- SDK declarations only this row uses (generated together with rw37_slim.h: do not edit between the markers) ---- */
#define rwID_GPVSPLUGIN MAKECHUNKID(rwVENDORID_CRITERIONTK, 0x2a)
/* ---- end of the generated declarations ---- */

#define rwPLUGIN_ID rwID_GPVSPLUGIN
/* TIER2: unvalidated authority=diffemu */
RpWorldSector * PVSGetNumSectors(RpWorldSector * worldSector, void *pData)
{

    if (worldSector && pData)
    {
        
        (*(RwInt32 *) pData)++;

        return (worldSector);
    }

    RWERROR((E_RW_NULLP));
    return ((RpWorldSector *)NULL);
}
