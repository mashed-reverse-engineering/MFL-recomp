/* rwa_virtualvoiceobj_005b6d70 (MFL.exe 0x005b6d70): RenderWare Audio (Criterion Software), neutral (codfh sub_23BBA0)
 * Source: codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32 (src/rwa/rwavirtualvoiceobj.c, function sub_23BBA0), a byte-identical
 * decompilation of the Xbox build of RenderWare Audio made with the same compiler (cl 13.00.9466). Used with
 * credit; split into one file per function. Changes: function names (sub_23BBA0 -> rwa_virtualvoiceobj_005b6d70; sub_23F960 -> RwaObjGetParamUInt32), the unit's data definitions are
 * extern declarations here, the CODFH_SP (single player) conditionals are resolved for the multiplayer build.
 * Byte-identical to MFL.exe 0x005b6d70 (tools/cmpfn.py, relocations masked). */
#include <string.h>
#include "rwa.h"
#include "rwaint.h"
extern RwInt32 RwaObjGetParamUInt32(RwaObjHandle *handle, RwInt32 param, RwInt32 type, RwInt32 value);
rwaEmitter *rwa_virtualvoiceobj_005b6d70(rwaEmitter *emitter, const RwaUUID *param, RwUInt32 *value);

rwaEmitter *
rwa_virtualvoiceobj_005b6d70(rwaEmitter *emitter, const RwaUUID *param, RwUInt32 *value)
{
    emitter->sound.unk48 = (emitter->sound.handle && (emitter->sound.flags & 2)) ?
        RwaObjGetParamUInt32(emitter->sound.handle, 7, 2, 0) : emitter->sound.unk48;
    *value = emitter->sound.unk48;

    return emitter;
}
