/* RwaObjIntGetParamUInt32 (MFL.exe 0x005c1f30): RenderWare Audio (Criterion Software), codfh name
 * Source: codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32 (src/rwa/unit_246FD0.c, function RwaObjIntGetParamUInt32), a byte-identical
 * decompilation of the Xbox build of RenderWare Audio made with the same compiler (cl 13.00.9466). Used with
 * credit; split into one file per function. Changes: function names (sub_247260 -> rwa_unit246fd0_005c1eb0), the unit's data definitions are
 * extern declarations here, the CODFH_SP (single player) conditionals are resolved for the multiplayer build.
 * Byte-identical to MFL.exe 0x005c1f30 (tools/cmpfn.py, relocations masked). */
#include "rwa.h"
#include "rwaint.h"
extern RwBool rwa_unit246fd0_005c1eb0(rwaDecoder *obj, rwaObjInterface *iface, RwInt32 type, void *value);

RwUInt32
RwaObjIntGetParamUInt32(rwaDecoder *obj, rwaObjInterface *iface, RwUInt32 type)
{
    rwa_unit246fd0_005c1eb0(obj, iface, type, &type);
    return type;
}
