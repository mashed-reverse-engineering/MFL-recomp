/* RwaFindLSB (MFL.exe 0x005bf560): RenderWare Audio (Criterion Software), rwa.dll 1.0
 * Source: codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32 (src/rwa/unit_241F90.c, function sub_242150), a byte-identical
 * decompilation of the Xbox build of RenderWare Audio made with the same compiler (cl 13.00.9466). Used with
 * credit; split into one file per function. Changes: function names (sub_242150 -> RwaFindLSB), the unit's data definitions are
 * extern declarations here, the CODFH_SP (single player) conditionals are resolved for the multiplayer build.
 * Byte-identical to MFL.exe 0x005bf560 (tools/cmpfn.py, relocations masked). */
#include "rwa.h"
#include <math.h>
#include "rwaint.h"

RwUInt8
RwaFindLSB(RwUInt32 value)
{
    RwUInt8     bit;

    for (bit = 0; bit < 32; bit++)
    {
        if (value & (1 << bit))
        {
            break;
        }
    }

    return bit;
}
