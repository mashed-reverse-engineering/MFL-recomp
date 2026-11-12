/* _rwPluginRegistrySkipDataChunks (MFL.exe 0x004f2130): RenderWare Graphics 3.7 (Criterion Software), finds the extension chunk of an
 * object in a stream and skips all the plugin chunks inside it.
 * Adapted from codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32, src/rw/src/plcore/batkbin.c @e578672,
 * not byte-identical, changes: RWFUNCTION / RWASSERT / RWRETURN expanded (release build), the rwID_EXTENSION / rwCHUNKHEADERSIZE
 * constants inlined, the registry is only passed through (no field access, so the type is left opaque).
 * Verified by tools/diffemu (tier 2). */
#include "mfl_tier2.h"

#define rwID_EXTENSION 0x03
#define rwCHUNKHEADERSIZE 12

typedef struct RwPluginRegistry RwPluginRegistry;

/*004dc720*/ extern RwBool RwStreamFindChunk(RwStream *stream, RwUInt32 type, RwUInt32 *lengthOut, RwUInt32 *versionOut);
/*004dc540*/ extern RwBool _rwStreamReadChunkHeader(RwStream *stream, RwUInt32 *type, RwUInt32 *length, RwUInt32 *version, RwUInt32 *buildNum);
/*004dc190*/ extern RwStream *RwStreamSkip(RwStream *stream, RwUInt32 offset);

/* TIER2: validated 2026-11-10 authority=diffemu */
const RwPluginRegistry *_rwPluginRegistrySkipDataChunks(const RwPluginRegistry *reg, RwStream *stream)
{
    RwUInt32 length;

    /* Since we are skipping, and not interpreting these chunks, we don't care about version numbers */
    /* Find the extension chunk and get its length */
    if (!RwStreamFindChunk(stream, rwID_EXTENSION, &length, (RwUInt32 *)0))
    {
        return 0;
    }

    /* Read a chunk from the stream, and then skip it. */
    while (length > 0)
    {
        /* Read a chunk header */
        RwUInt32 readLength;

        if (!_rwStreamReadChunkHeader(stream, (RwUInt32 *)0, &readLength, (RwUInt32 *)0, (RwUInt32 *)0))
        {
            return 0;
        }

        if (!RwStreamSkip(stream, readLength))
        {
            return 0;
        }

        length -= (readLength + rwCHUNKHEADERSIZE);
    }

    return reg;
}
