/* rwa_unit249490_005c7b00 (MFL.exe 0x005c7b00): RenderWare Audio (Criterion Software), neutral (codfh sub_24C700)
 * Source: codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32 (src/rwa/unit_249490.c, function sub_24C700), a byte-identical
 * decompilation of the Xbox build of RenderWare Audio made with the same compiler (cl 13.00.9466). Used with
 * credit; split into one file per function. Changes: function names (sub_24BDE0 -> rwa_unit249490_005c7200; sub_24C700 -> rwa_unit249490_005c7b00), the unit's data definitions are
 * extern declarations here, the CODFH_SP (single player) conditionals are resolved for the multiplayer build.
 * Byte-identical to MFL.exe 0x005c7b00 (tools/cmpfn.py, relocations masked). */
#include "rwa.h"
#include "rwaint.h"
#include <math.h>
extern void rwa_unit249490_005c7200(RwUInt32 code, RwInt32 *predicted, RwInt32 *index);

/* IMA ADPCM decode, stereo: per block, each channel's header (first
 * sample and step index), then eight pairs of nibble words. */
RwBool
rwa_unit249490_005c7b00(const RwUInt32 *src, RwInt16 *dest, RwUInt32 blocks)
{
    const RwUInt32 *in = src;
    RwInt16    *out = dest;

    while (blocks)
    {
        RwUInt32    left;
        RwUInt32    right;
        RwInt32     predictedLeft;
        RwInt32     predictedRight;
        RwInt32     indexLeft;
        RwInt32     indexRight;
        RwInt32     j;

        blocks--;
        left = *in++;
        right = *in++;
        predictedLeft = (RwInt16)left;
        predictedRight = (RwInt16)right;
        indexLeft = (left >> 16) & 0x7F;
        indexRight = (right >> 16) & 0x7F;
        if (indexLeft < 0 || indexLeft > 88 || indexRight < 0 || indexRight > 88)
        {
            return FALSE;
        }
        *out++ = (RwInt16)predictedLeft;
        *out++ = (RwInt16)predictedRight;
        for (j = 0; j < 8; j++)
        {
            RwUInt32    dataLeft = *in++;
            RwUInt32    dataRight = *in++;
            RwInt32     k;

            for (k = 0; k < 7 + (j != 7); k++)
            {
                rwa_unit249490_005c7200(dataLeft & 0xF, &predictedLeft, &indexLeft);
                rwa_unit249490_005c7200(dataRight & 0xF, &predictedRight, &indexRight);
                *out++ = (RwInt16)predictedLeft;
                *out++ = (RwInt16)predictedRight;
                dataLeft >>= 4;
                dataRight >>= 4;
            }
        }
    }

    return TRUE;
}
