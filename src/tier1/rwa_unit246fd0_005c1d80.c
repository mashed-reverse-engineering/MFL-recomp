/* rwa_unit246fd0_005c1d80 (MFL.exe 0x005c1d80): RenderWare Audio (Criterion Software), neutral (codfh sub_247120)
 * Source: codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32 (src/rwa/unit_246FD0.c, function sub_247120), a byte-identical
 * decompilation of the Xbox build of RenderWare Audio made with the same compiler (cl 13.00.9466). Used with
 * credit; split into one file per function. Changes: function names (sub_243B80 -> RwaObjInterfaceRemove; sub_247120 -> rwa_unit246fd0_005c1d80), the unit's data definitions are
 * extern declarations here, the CODFH_SP (single player) conditionals are resolved for the multiplayer build.
 * Byte-identical to MFL.exe 0x005c1d80 (tools/cmpfn.py, relocations masked). */
#include "rwa.h"
#include "rwaint.h"
extern RwInt32 RwaObjInterfaceRemove(void *objInterface);

void
rwa_unit246fd0_005c1d80(rwaDecoder *obj, RwUInt32 unused)
{
    RwaObjInterfaceRemove(obj->unk28);
}
