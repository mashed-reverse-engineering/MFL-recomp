/* rwa_virtualvoiceobj_005b6c20 (MFL.exe 0x005b6c20): RenderWare Audio (Criterion Software), neutral (codfh sub_23BA70)
 * Source: codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32 (src/rwa/rwavirtualvoiceobj.c, function sub_23BA70), a byte-identical
 * decompilation of the Xbox build of RenderWare Audio made with the same compiler (cl 13.00.9466). Used with
 * credit; split into one file per function. Changes: function names (sub_23BA70 -> rwa_virtualvoiceobj_005b6c20), the unit's data definitions are
 * extern declarations here, the CODFH_SP (single player) conditionals are resolved for the multiplayer build.
 * Byte-identical to MFL.exe 0x005b6c20 (tools/cmpfn.py, relocations masked). */
#include <string.h>
#include "rwa.h"
#include "rwaint.h"
extern void RwaObjHandleSetParamUInt8(RwaObjHandle *handle, RwInt32 param, RwInt32 type, RwUInt32 value);
rwaEmitter *rwa_virtualvoiceobj_005b6c20(rwaEmitter *emitter, const RwaUUID *param, const RwUInt32 *value);

rwaEmitter *
rwa_virtualvoiceobj_005b6c20(rwaEmitter *emitter, const RwaUUID *param, const RwUInt32 *value)
{
    emitter->sound.unk74 = *value;

    if (emitter->sound.handle)
    {
        RwaObjHandleSetParamUInt8(emitter->sound.handle, 3, 1, *value);
    }

    return emitter;
}
