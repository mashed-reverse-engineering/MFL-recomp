/* rpReadAtomicRights (MFL.exe 004f6090). Adapted from codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32, src/rw/world/baclump.c @e578672
 *, not byte-identical, changes: cut down to this function and the private declarations it uses, the file
 * statics declared extern at their MFL.exe addresses, release build macros expanded (RWFUNCTION / RWASSERT dropped, RWRETURN -> return),
 * conditional compilation resolved for the release D3D9 build, the SDK includes replaced by rw37_slim.h, data and functions MFL.exe names
 * declared extern at their addresses. Generated, then hand-checked; compared with MFL.exe by differential emulation (tier 2). */
#include "rw37_slim.h"
/* ---- SDK declarations only this row uses (generated together with rw37_slim.h: do not edit between the markers) ---- */
extern RwStream *RwStreamReadInt32(RwStream *stream, RwInt32 *ints,
                                   RwUInt32 numBytes);
/* ---- end of the generated declarations ---- */

/* baclump.c's file statics, MFL.exe data at 0x7d532c / 0x7d5304 (rows_aliases.txt) */
extern RwUInt32 lastSeenRightsPluginId;
extern RwUInt32 lastSeenExtraData;
/* TIER2: unvalidated authority=diffemu */
RwStream* rpReadAtomicRights(RwStream *s, RwInt32 len, void *obj __RWUNUSED__, RwInt32 off __RWUNUSED__, RwInt32 size __RWUNUSED__)
{

    if (!RwStreamReadInt32(s, (RwInt32*)&(lastSeenRightsPluginId),
                           sizeof(RwInt32)))
    {
        return ((RwStream *) NULL);
    }
    if (len == 8)
    {
        if (!RwStreamReadInt32(s, (RwInt32*)&(lastSeenExtraData),
                               sizeof(RwInt32)))
        {
            return ((RwStream *) NULL);
        }
    }
    return (s);
}
