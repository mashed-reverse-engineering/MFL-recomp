/* rwa_unit2fd5d0_005da750 (MFL.exe 0x005da750): RenderWare Audio (Criterion Software), neutral (codfh sub_2FD6F0)
 * Source: codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32 (src/rwa/unit_2FD5D0.c, function sub_2FD6F0), a byte-identical
 * decompilation of the Xbox build of RenderWare Audio made with the same compiler (cl 13.00.9466). Used with
 * credit; split into one file per function. Changes: function names (sub_2FD6F0 -> rwa_unit2fd5d0_005da750), the unit's data definitions are
 * extern declarations here, the CODFH_SP (single player) conditionals are resolved for the multiplayer build.
 * Byte-identical to MFL.exe 0x005da750 (tools/cmpfn.py, relocations masked). */
#include "rwa.h"
#include "rwaint.h"

/* Sets (value non-zero) or clears count bits from bit start. */
void
rwa_unit2fd5d0_005da750(RwaAllocBitmap *bitmap, RwUInt32 start, RwUInt32 count, RwUInt32 value)
{
    RwUInt8     set = (value != 0);
    RwUInt8     firstBit = (RwUInt8)(start & 7);
    RwUInt32    first = start >> 3;
    RwUInt32    last = (start + count) >> 3;
    RwUInt8     lastBit = (RwUInt8)((start + count) & 7);
    RwUInt32    i;

    for (i = first; i <= last; i++)
    {
        RwUInt8     lo = 0;
        RwUInt8     hi = 8;

        if (i == first)
        {
            lo = firstBit;
        }
        if (i == last)
        {
            hi = lastBit;
        }

        if (lo == 0 && hi == 8)
        {
            bitmap->bits[i] = ~(RwUInt8)(set - 1);
        }
        else
        {
            RwUInt32    bit;

            for (bit = lo; bit < hi; bit++)
            {
                bitmap->bits[i] &= ~(1 << bit);
                bitmap->bits[i] |= set << bit;
            }
        }
    }
}
