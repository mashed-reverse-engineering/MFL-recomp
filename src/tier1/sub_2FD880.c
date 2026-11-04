/* sub_2FD880 (MFL.exe 0x005da800): RenderWare Audio (Criterion Software), 
 * Source: codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32 (src/rwa/unit_2FD5D0.c, function sub_2FD880), a byte-identical
 * decompilation of the Xbox build of RenderWare Audio made with the same compiler (cl 13.00.9466; MFL.exe was built with cl 13.10). Used with
 * credit; split into one file per function. Changes: function names (sub_2FD6F0 -> rwa_unit2fd5d0_005da750), the unit's data definitions are
 * extern declarations here, the CODFH_SP (single player) conditionals are resolved for the multiplayer build.
 * Byte-identical to MFL.exe 0x005da800 (tools/cmpfn.py, relocations masked). */
/* compile: cl1310 */
#include "rwa2.h"
#include "rwaint2.h"
extern void rwa_unit2fd5d0_005da750(RwaAllocBitmap *bitmap, RwUInt32 start, RwUInt32 count, RwUInt32 value);

/* Frees a run sub_2FD880 allocated. */
/* _rwaAllocBitmapAlloc, rwad.dll 0x100BB929: the compiler unrolls
 * the ordinary bit loop. Clamp the run before breaking the byte loop. */
RwBool
sub_2FD880(RwaAllocBitmap *bitmap, RwUInt32 count, RwUInt32 *handle)
{
    RwUInt32 start = 0;
    RwUInt32 run = 0;
    RwUInt32 longest = 0;
    RwUInt32 i;

    for (i = 0; i < bitmap->numBytes; i++)
    {
        if (!bitmap->bits[i])
        {
            if (!run)
            {
                start = i * 8;
            }
            run += 8;
        }
        else if (bitmap->bits[i] != 0xFF)
        {
            RwUInt32 bit;
            for (bit = 0; bit < 8; bit++)
            {
                if (!((1 << bit) & bitmap->bits[i]))
                {
                    if (!run)
                    {
                        start = bit + i * 8;
                    }
                    run++;
                }
                else
                {
                    start = bit + i * 8;
                    run = 0;
                }
                if (run == count)
                {
                    break;
                }
            }
        }
        else
        {
            start = i * 8;
            run = 0;
        }
        if (run > longest)
        {
            longest = run;
        }
        if (run >= count)
        {
            run = count;
            break;
        }
    }
    if (run < count)
    {
        *handle = 0;
        return FALSE;
    }
    rwa_unit2fd5d0_005da750(bitmap, start, run, 1);
    *handle = start | (run << bitmap->shift);
    return TRUE;
}
