/* _rwaWaveFormatFindUUID (MFL.exe 0x005bd050): RenderWare Audio (Criterion Software), rwa.dll 0.957
 * Source: codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32 (src/rwa/unit_242910.c, function sub_242DE0), a byte-identical
 * decompilation of the Xbox build of RenderWare Audio made with the same compiler (cl 13.00.9466). Used with
 * credit; split into one file per function. Changes: function names (sub_242DE0 -> _rwaWaveFormatFindUUID), the unit's data definitions are
 * extern declarations here, the CODFH_SP (single player) conditionals are resolved for the multiplayer build.
 * Byte-identical to MFL.exe 0x005bd050 (tools/cmpfn.py, relocations masked). */
#include "rwa.h"
#include "rwaint.h"
extern const RwaUUID *unk_42F7A0[9];    /* the codecs */
extern const RwaUUID *unk_42F7A0[9];

const RwaUUID *
_rwaWaveFormatFindUUID(const RwaUUID *uuid)
{
    RwUInt32    i;

    for (i = 0; i < 9; i++)
    {
        if (!RwaUUIDCompare(unk_42F7A0[i], uuid))
        {
            return unk_42F7A0[i];
        }
    }

    return NULL;
}
