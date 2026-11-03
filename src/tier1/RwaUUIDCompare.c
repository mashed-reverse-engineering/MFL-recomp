/* RwaUUIDCompare (MFL.exe 0x005be600): RenderWare Audio (Criterion Software), codfh name
 * Source: codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32 (src/rwa/rwauuid.c, function RwaUUIDCompare), a byte-identical
 * decompilation of the Xbox build of RenderWare Audio made with the same compiler (cl 13.00.9466). Used with
 * credit; split into one file per function. Changes: function names (none), the unit's data definitions are
 * extern declarations here, the CODFH_SP (single player) conditionals are resolved for the multiplayer build.
 * Byte-identical to MFL.exe 0x005be600 (tools/cmpfn.py, relocations masked). */
#include "rwa.h"
#include "rwaint.h"
extern RwInt32 memcmp(const void *a, const void *b, RwUInt32 size);

RwInt32
RwaUUIDCompare(const RwaUUID *uuid1, const RwaUUID *uuid2)
{
    if (uuid1 == uuid2)
    {
        return 0;
    }

    return memcmp(uuid1, uuid2, sizeof(RwaUUID));
}
