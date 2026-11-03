/* rwa_unit249490_005c7a20 (MFL.exe 0x005c7a20): RenderWare Audio (Criterion Software), neutral (codfh sub_24C620)
 * Source: codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32 (src/rwa/unit_249490.c, function sub_24C620), a byte-identical
 * decompilation of the Xbox build of RenderWare Audio made with the same compiler (cl 13.00.9466). Used with
 * credit; split into one file per function. Changes: function names (sub_24BD30 -> rwa_unit249490_005c7150; sub_24C620 -> rwa_unit249490_005c7a20), the unit's data definitions are
 * extern declarations here, the CODFH_SP (single player) conditionals are resolved for the multiplayer build.
 * Byte-identical to MFL.exe 0x005c7a20 (tools/cmpfn.py, relocations masked). */
#include "rwa.h"
#include "rwaint.h"
#include <math.h>
extern RwUInt8 rwa_unit249490_005c7150(RwInt16 sample, RwInt32 *predicted, RwInt32 *index);

RwUInt32
rwa_unit249490_005c7a20(const RwInt16 *src, RwUInt32 *dest, RwInt32 count)
{
    const RwInt16 *in;
    RwUInt32   *out;
    RwUInt32    blocks;
    RwUInt32    size;
    RwInt32     index;
    RwInt32     predicted;
    RwInt32     remaining;
    RwUInt32    b;

    blocks = (RwUInt32)(count + 63) / 64;
    size = blocks * 36;
    in = src;
    out = dest;
    remaining = count;
    index = 0;

    for (b = blocks; b; b--)
    {
        RwUInt32    data;
        RwInt32     n;

        predicted = *in;
        in++;
        remaining--;
        *out++ = (index << 16) | (RwUInt16)predicted;

        data = 0;
        for (n = 0; n < 64; n++)
        {
            RwInt32     sample;

            data >>= 4;
            if (remaining > 0 && n < 63)
            {
                sample = *in++;
                remaining--;
            }
            else
            {
                sample = 0;
            }
            data |= (RwUInt32)rwa_unit249490_005c7150(sample, &predicted, &index) << 28;
            if ((n & 7) == 7)
            {
                *out++ = data;
                data = 0;
            }
        }
    }

    return size;
}
