/* rwa_streamsourceobj_005c8950 (MFL.exe 0x005c8950): RenderWare Audio (Criterion Software), neutral (codfh sub_240D80)
 * Source: codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32 (src/rwa/rwastreamsourceobj.c, function sub_240D80), a byte-identical
 * decompilation of the Xbox build of RenderWare Audio made with the same compiler (cl 13.00.9466). Used with
 * credit; split into one file per function. Changes: function names (sub_240D80 -> rwa_streamsourceobj_005c8950), the unit's data definitions are
 * extern declarations here, the CODFH_SP (single player) conditionals are resolved for the multiplayer build.
 * Byte-identical to MFL.exe 0x005c8950 (tools/cmpfn.py, relocations masked). */
#include "rwa.h"
#include "rwaint.h"
extern void RwaStreamNodeGetParams(rwaDecoder *obj, rwaStreamNodeParams *params);
extern void _rwaStreamSourceCreationParamsInit(rwaStreamSourceParams *params);

rwaDecoder *
rwa_streamsourceobj_005c8950(rwaDecoder *obj, rwaStreamNodeParams *params)
{
    rwaStreamSourceParams   defaults;

    RwaStreamNodeGetParams(obj, params);
    params->unk00 = 0;
    _rwaStreamSourceCreationParamsInit(&defaults);

    params->source[0] = defaults.values[0];
    params->source[6] = defaults.values[6];
    params->source[7] = defaults.values[7];
    params->source[2] = defaults.values[2];
    params->source[4] = defaults.values[4];
    params->source[5] = defaults.values[5];
    params->source[3] = defaults.values[3];
    params->source[1] = defaults.values[1];
    params->source[14] = defaults.values[14];
    params->source[13] = defaults.values[13];
    params->source[8] = defaults.values[8];
    params->source[9] = defaults.values[9];
    params->source[11] = defaults.values[11];
    params->source[10] = defaults.values[10];

    return obj;
}
