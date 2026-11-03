/* rwa_unit246d50_005c1c10 (MFL.exe 0x005c1c10): RenderWare Audio (Criterion Software), neutral (codfh sub_246FB0)
 * Source: codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32 (src/rwa/unit_246D50.c, function sub_246FB0), a byte-identical
 * decompilation of the Xbox build of RenderWare Audio made with the same compiler (cl 13.00.9466). Used with
 * credit; split into one file per function. Changes: function names (sub_246FB0 -> rwa_unit246d50_005c1c10), the unit's data definitions are
 * extern declarations here, the CODFH_SP (single player) conditionals are resolved for the multiplayer build.
 * Byte-identical to MFL.exe 0x005c1c10 (tools/cmpfn.py, relocations masked). */
#include "rwa.h"
#include "rwaint.h"

void *
rwa_unit246d50_005c1c10(RwUInt32 size, RwUInt32 unused)
{
    return sub_242170(size, rwaMEMHINTDUR_EVENT | rwaID_80F);
}
