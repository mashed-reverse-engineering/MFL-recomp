/* rwa_unit249490_005c67c0 (MFL.exe 0x005c67c0): RenderWare Audio (Criterion Software), neutral (codfh sub_24B260)
 * Source: codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32 (src/rwa/unit_249490.c, function sub_24B260), a byte-identical
 * decompilation of the Xbox build of RenderWare Audio made with the same compiler (cl 13.00.9466). Used with
 * credit; split into one file per function. Changes: function names (sub_24B260 -> rwa_unit249490_005c67c0), the unit's data definitions are
 * extern declarations here, the CODFH_SP (single player) conditionals are resolved for the multiplayer build.
 * Byte-identical to MFL.exe 0x005c67c0 (tools/cmpfn.py, relocations masked). */
#include "rwa.h"
#include "rwaint.h"
#include <math.h>
#define RwaDspSwap16Macro(v) ((((v) & 0xFF00) >> 8) | (((v) & 0xFF) << 8))
#define RwaDspSwap32Macro(v) ((((v) & 0xFF000000) >> 24) | (((v) & 0xFF0000) >> 8) | (((v) & 0xFF00) << 8) | (((v) & 0xFF) << 24))
extern void rwa_unit249490_005c67c0(const RwaWaveFormat *format, const RwUInt8 *src, RwInt16 *dest);

/* Debug reference: rwad.dll _RwaWaveConvGCNADPCMDecode at 0x100B007E. */
void
rwa_unit249490_005c67c0(const RwaWaveFormat *format, const RwUInt8 *src, RwInt16 *dest)
{
    const RwUInt8 *header;
    RwUInt32 number;
    RwUInt32 remaining;
    RwUInt32 count;
    RwInt32 result;
    RwInt16 coef[16];
    RwInt16 sample;
    RwUInt32 shift;
    RwInt16 c1;
    RwInt16 c2;
    RwUInt32 i;
    RwUInt32 j;
    RwUInt32 frames;
    RwInt32 hist1;
    RwInt32 hist2;
    RwUInt32 filter;
    const RwUInt8 *data;
    RwInt16 *out;

    header = (const RwUInt8 *)format->extra;
    number = *(const RwUInt32 *)header;
    number = RwaDspSwap32Macro(number);
    remaining = number - 2;
    for (i = 0; i < 16; i++)
    {
        sample = ((const RwInt16 *)(header + 0x1C))[i];
        sample = RwaDspSwap16Macro(sample);
        coef[i] = sample;
    }
    sample = *(const RwInt16 *)(header + 0x40);
    sample = RwaDspSwap16Macro(sample);
    hist2 = sample;
    sample = *(const RwInt16 *)(header + 0x42);
    sample = RwaDspSwap16Macro(sample);
    hist1 = sample;
    frames = (remaining + 13) / 14;
    data = src;
    out = dest;
    *out++ = (RwInt16)hist2;
    *out++ = (RwInt16)hist1;
    for (i = 0; i < frames; i++)
    {
        filter = *data >> 4;
        shift = *data & 15;
        c1 = coef[filter << 1];
        c2 = coef[(filter << 1) + 1];
        data++;
        count = 14;
        if (remaining < 14)
        {
            count = remaining;
        }
        remaining -= count;
        for (j = 0; j < count; j++)
        {
            if (!(j & 1))
            {
                sample = (*data & 0xF0) << 8;
                sample >>= 12;
                result = (sample << shift) + ((c1 * hist1) >> 11) + ((c2 * hist2) >> 11);
                if (result > 32767)
                    result = 32767;
                else if (result < -32768)
                    result = -32768;
                *out++ = (RwInt16)result;
                hist2 = hist1;
                hist1 = result;
            }
            else
            {
                sample = (*data & 15) << 12;
                sample >>= 12;
                result = (sample << shift) + ((c1 * hist1) >> 11) + ((c2 * hist2) >> 11);
                if (result > 32767)
                    result = 32767;
                else if (result < -32768)
                    result = -32768;
                *out++ = (RwInt16)result;
                hist2 = hist1;
                hist1 = result;
                data++;
            }
        }
    }
}
