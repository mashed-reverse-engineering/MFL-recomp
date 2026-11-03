/* _rwaWaveFormatFreeMiscData (MFL.exe 0x005bcd50): RenderWare Audio (Criterion Software), rwa.dll 0.914
 * Source: codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32 (src/rwa/unit_242910.c, function sub_242B30), a byte-identical
 * decompilation of the Xbox build of RenderWare Audio made with the same compiler (cl 13.00.9466). Used with
 * credit; split into one file per function. Changes: function names (sub_242B30 -> _rwaWaveFormatFreeMiscData), the unit's data definitions are
 * extern declarations here, the CODFH_SP (single player) conditionals are resolved for the multiplayer build.
 * Byte-identical to MFL.exe 0x005bcd50 (tools/cmpfn.py, relocations masked). */
#include "rwa.h"
#include "rwaint.h"

void
_rwaWaveFormatFreeMiscData(RwaWaveFormat *format)
{
    if (!(format->flags & 2) && format->extra)
    {
        sub_2421B0(format->extra);
    }

    format->extra = NULL;
    format->extraSize = 0;
}
