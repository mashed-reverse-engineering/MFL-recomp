/* _rpReadMaterialRights (MFL.exe 004f7ff0). Adapted from codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32, src/rw/world/bamateri.c @e578672,
 * not byte-identical, changes: cut down to this function and the private declarations it uses, release build macros expanded (RWFUNCTION /
 * RWASSERT dropped, RWRETURN -> return), conditional compilation resolved for the release D3D9 build, the SDK includes replaced by rw37_slim.h,
 * data and functions MFL.exe names declared extern at their addresses. Generated, then hand-checked; compared with MFL.exe by differential
 * emulation (tier 2). Hand changes: the file statics are the MFL.exe words; _rpReadWorldRights (0x4f9a90) and _rpReadSectRights (0x4f9b30),
 * the same reader over the world / sector statics, written from the MFL.exe disassembly in the same form. */
#include "rw37_slim.h"
/* ---- SDK declarations only this row uses (generated together with rw37_slim.h: do not edit between the markers) ---- */
extern RwStream *RwStreamReadInt32(RwStream *stream, RwInt32 *ints,
                                   RwUInt32 numBytes);
/* ---- end of the generated declarations ---- */

/* the rights readers' file statics live in MFL.exe (RwStreamReadInt32 targets in the original) */
extern RwUInt32 g_rp_7d538c;      /* bamateri.c lastSeenRightsPluginId */
extern RwUInt32 g_rp_7d5364;      /* bamateri.c lastSeenExtraData */
extern RwUInt32 lastSeenWorldRightsPluginId;
extern RwUInt32 lastSeenWorldExtraData;
extern RwUInt32 lastSeenSectRightsPluginId;
extern RwUInt32 lastSeenSectExtraData;

#define RIGHTSREAD(s, len, pluginId, extraData)                                        \
    if (!RwStreamReadInt32((s), (RwInt32 *)&(pluginId), sizeof(RwInt32)))              \
    {                                                                                  \
        return ((RwStream *) NULL);                                                    \
    }                                                                                  \
    if ((len) == 8)                                                                    \
    {                                                                                  \
        if (!RwStreamReadInt32((s), (RwInt32 *)&(extraData), sizeof(RwInt32)))         \
        {                                                                              \
            return ((RwStream *) NULL);                                                \
        }                                                                              \
    }                                                                                  \
    return (s)

/* TIER2: unvalidated authority=diffemu */
/* TIER2: unvalidated 2026-11-14 authority=diffemu */
RwStream* _rpReadMaterialRights(RwStream *s, RwInt32 len, void *obj __RWUNUSED__, RwInt32 off __RWUNUSED__, RwInt32 size __RWUNUSED__)
{
    RIGHTSREAD(s, len, g_rp_7d538c, g_rp_7d5364);
}

/* TIER2: unvalidated authority=diffemu */
/* TIER2: unvalidated 2026-11-14 authority=diffemu */
RwStream* _rpReadWorldRights(RwStream *s, RwInt32 len, void *obj __RWUNUSED__, RwInt32 off __RWUNUSED__, RwInt32 size __RWUNUSED__)
{
    RIGHTSREAD(s, len, lastSeenWorldRightsPluginId, lastSeenWorldExtraData);
}

/* TIER2: unvalidated authority=diffemu */
/* TIER2: unvalidated 2026-11-14 authority=diffemu */
RwStream* _rpReadSectRights(RwStream *s, RwInt32 len, void *obj __RWUNUSED__, RwInt32 off __RWUNUSED__, RwInt32 size __RWUNUSED__)
{
    RIGHTSREAD(s, len, lastSeenSectRightsPluginId, lastSeenSectExtraData);
}
