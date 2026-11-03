/* rwa_unit244790_005bb3e0 (MFL.exe 0x005bb3e0): RenderWare Audio (Criterion Software), neutral (codfh sub_244800)
 * Source: codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32 (src/rwa/unit_244790.c, function sub_244800), a byte-identical
 * decompilation of the Xbox build of RenderWare Audio made with the same compiler (cl 13.00.9466). Used with
 * credit; split into one file per function. Changes: function names (sub_244800 -> rwa_unit244790_005bb3e0), the unit's data definitions are
 * extern declarations here, the CODFH_SP (single player) conditionals are resolved for the multiplayer build.
 * Byte-identical to MFL.exe 0x005bb3e0 (tools/cmpfn.py, relocations masked). */
#include "rwa.h"
#include "rwaint.h"
extern void sub_2421B0(void *mem);

void
rwa_unit244790_005bb3e0(RwaWave *wave)
{
    if (wave->buffer)
    {
        sub_2421B0(wave->buffer);
    }
}
