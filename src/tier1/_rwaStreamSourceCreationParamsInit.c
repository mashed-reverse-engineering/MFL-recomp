/* _rwaStreamSourceCreationParamsInit (MFL.exe 0x005c2050): RenderWare Audio (Criterion Software), codfh name
 * Source: codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32 (src/rwa/unit_246FD0.c, function _rwaStreamSourceCreationParamsInit), a byte-identical
 * decompilation of the Xbox build of RenderWare Audio made with the same compiler (cl 13.00.9466). Used with
 * credit; split into one file per function. Changes: function names (sub_2473D0 -> rwa_unit246fd0_005c2040), the unit's data definitions are
 * extern declarations here, the CODFH_SP (single player) conditionals are resolved for the multiplayer build.
 * Byte-identical to MFL.exe 0x005c2050 (tools/cmpfn.py, relocations masked). */
#include "rwa.h"
#include "rwaint.h"
extern void sub_246FC0(void *mem, RwUInt32 unused);
extern void * rwa_unit246fd0_005c2040(RwUInt32 size, RwUInt32 unused);

rwaStreamSourceParams *
_rwaStreamSourceCreationParamsInit(rwaStreamSourceParams *params)
{
    params->values[0] = (RwUInt32)rwa_unit246fd0_005c2040;
    params->values[1] = (RwUInt32)sub_246FC0;
    params->values[2] = 0;
    params->callback = NULL;
    params->data = NULL;
    params->values[4] = 0;
    params->values[6] = 0;
    params->values[7] = 0;
    params->values[5] = 0;
    params->values[10] = 0;
    params->values[11] = 0;
    params->values[13] = 0;
    params->values[14] = 0;
    params->values[8] = 0;
    params->values[9] = 0;
    params->values[12] = 0;
    params->values[3] = 0;
    return params;
}
