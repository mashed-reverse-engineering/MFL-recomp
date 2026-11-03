/* rwa_unit2449c0_005bb6b0 (MFL.exe 0x005bb6b0): RenderWare Audio (Criterion Software), neutral (codfh sub_244A20)
 * Source: codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32 (src/rwa/unit_2449C0.c, function sub_244A20), a byte-identical
 * decompilation of the Xbox build of RenderWare Audio made with the same compiler (cl 13.00.9466). Used with
 * credit; split into one file per function. Changes: function names (sub_244A20 -> rwa_unit2449c0_005bb6b0), the unit's data definitions are
 * extern declarations here, the CODFH_SP (single player) conditionals are resolved for the multiplayer build.
 * Byte-identical to MFL.exe 0x005bb6b0 (tools/cmpfn.py, relocations masked). */
#include "rwa.h"
#include "rwaint.h"

RwUInt32
rwa_unit2449c0_005bb6b0(rwaCacheClient *client)
{
    return client->request.offset;
}
