/* rwa_unit2422b0_005bc260 (MFL.exe 0x005bc260): RenderWare Audio (Criterion Software), neutral (codfh sub_242570)
 * Source: codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32 (src/rwa/unit_2422B0.c, function sub_242570), a byte-identical
 * decompilation of the Xbox build of RenderWare Audio made with the same compiler (cl 13.00.9466). Used with
 * credit; split into one file per function. Changes: function names (sub_242570 -> rwa_unit2422b0_005bc260), the unit's data definitions are
 * extern declarations here, the CODFH_SP (single player) conditionals are resolved for the multiplayer build.
 * Byte-identical to MFL.exe 0x005bc260 (tools/cmpfn.py, relocations masked). */
#include "rwa.h"
#include "rwaint.h"

RwBool
rwa_unit2422b0_005bc260(RwaWave *wave)
{
    /* The device-wide callback takes no wave: RwaWaveDefUploadBusyFunc in
     * docs/reference/rwacore_ps2.h; the image tail-calls it without arguments. */
    RwBool    (*busy)(void) = wave->waveDef->busy;

    if (!busy)
    {
        return FALSE;
    }

    return busy();
}
