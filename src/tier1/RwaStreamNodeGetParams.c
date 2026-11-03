/* RwaStreamNodeGetParams (MFL.exe 0x005c1d50): RenderWare Audio (Criterion Software), codfh name
 * Source: codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32 (src/rwa/unit_246FD0.c, function RwaStreamNodeGetParams), a byte-identical
 * decompilation of the Xbox build of RenderWare Audio made with the same compiler (cl 13.00.9466). Used with
 * credit; split into one file per function. Changes: function names (none), the unit's data definitions are
 * extern declarations here, the CODFH_SP (single player) conditionals are resolved for the multiplayer build.
 * Byte-identical to MFL.exe 0x005c1d50 (tools/cmpfn.py, relocations masked). */
#include "rwa.h"
#include "rwaint.h"

rwaDecoder *
RwaStreamNodeGetParams(rwaDecoder *obj, rwaStreamNodeParams *params)
{
    params->unk00 = (RwUInt32)-1;
    params->unk04 = (RwUInt32)-1;
    params->callback = NULL;
    params->unk08 = 0;
    params->unk0C = 1;
    params->unk14 = 0;
    params->unk18 = 0;
    return obj;
}
