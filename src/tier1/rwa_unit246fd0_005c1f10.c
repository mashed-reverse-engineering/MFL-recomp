/* rwa_unit246fd0_005c1f10 (MFL.exe 0x005c1f10): RenderWare Audio (Criterion Software), neutral (codfh sub_247290)
 * Source: codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32 (src/rwa/unit_246FD0.c, function sub_247290), a byte-identical
 * decompilation of the Xbox build of RenderWare Audio made with the same compiler (cl 13.00.9466). Used with
 * credit; split into one file per function. Changes: function names (sub_247220 -> rwa_unit246fd0_005c1e80; sub_247290 -> rwa_unit246fd0_005c1f10), the unit's data definitions are
 * extern declarations here, the CODFH_SP (single player) conditionals are resolved for the multiplayer build.
 * Byte-identical to MFL.exe 0x005c1f10 (tools/cmpfn.py, relocations masked). */
#include "rwa.h"
#include "rwaint.h"
extern RwBool rwa_unit246fd0_005c1e80(rwaDecoder *obj, rwaObjInterface *iface, RwInt32 type, void *value);

void
rwa_unit246fd0_005c1f10(rwaDecoder *obj, rwaObjInterface *iface, RwInt32 type, ...)
{
    rwa_unit246fd0_005c1e80(obj, iface, type, (&type) + 1);
}
