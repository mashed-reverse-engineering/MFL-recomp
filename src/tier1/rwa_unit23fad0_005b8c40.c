/* rwa_unit23fad0_005b8c40 (MFL.exe 0x005b8c40): RenderWare Audio (Criterion Software), neutral (codfh sub_240120)
 * Source: codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32 (src/rwa/unit_23FAD0.c, function sub_240120), a byte-identical
 * decompilation of the Xbox build of RenderWare Audio made with the same compiler (cl 13.00.9466). Used with
 * credit; split into one file per function. Changes: function names (sub_240120 -> rwa_unit23fad0_005b8c40), the unit's data definitions are
 * extern declarations here, the CODFH_SP (single player) conditionals are resolved for the multiplayer build.
 * Byte-identical to MFL.exe 0x005b8c40 (tools/cmpfn.py, relocations masked). */
#include "rwa.h"
#include "rwaint.h"

RwUInt32
rwa_unit23fad0_005b8c40(rwaStream *stream)
{
    return stream->unkD6C;
}
