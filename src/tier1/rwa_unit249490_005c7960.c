/* rwa_unit249490_005c7960 (MFL.exe 0x005c7960): RenderWare Audio (Criterion Software), neutral (codfh sub_24C560)
 * Source: codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32 (src/rwa/unit_249490.c, function sub_24C560), a byte-identical
 * decompilation of the Xbox build of RenderWare Audio made with the same compiler (cl 13.00.9466). Used with
 * credit; split into one file per function. Changes: function names (sub_24BDE0 -> rwa_unit249490_005c7200; sub_24C560 -> rwa_unit249490_005c7960), the unit's data definitions are
 * extern declarations here, the CODFH_SP (single player) conditionals are resolved for the multiplayer build.
 * Byte-identical to MFL.exe 0x005c7960 (tools/cmpfn.py, relocations masked). */
#include "rwa.h"
#include "rwaint.h"
#include <math.h>
extern void rwa_unit249490_005c7200(RwUInt32 code, RwInt32 *predicted, RwInt32 *index);

RwBool
rwa_unit249490_005c7960(const RwUInt32 *src, RwInt16 *dest, RwUInt32 blocks)
{
    const RwUInt32 *in = src;
    RwInt16    *out = dest;

    while (blocks)
    {
        RwUInt32    header;
        RwInt32     predicted;
        RwInt32     index;
        RwInt32     j;

        blocks--;
        header = *in++;
        predicted = (RwInt16)header;
        index = (header >> 16) & 0x7F;
        if (index < 0 || index > 88)
        {
            return FALSE;
        }
        *out++ = (RwInt16)predicted;
        for (j = 0; j < 8; j++)
        {
            RwUInt32    data = *in++;
            RwInt32     k;

            for (k = 0; k < 7 + (j != 7); k++)
            {
                rwa_unit249490_005c7200(data & 0xF, &predicted, &index);
                *out++ = (RwInt16)predicted;
                data >>= 4;
            }
        }
    }

    return TRUE;
}
