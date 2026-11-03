/* rwa_unit2452a0_005c01a0 (MFL.exe 0x005c01a0): RenderWare Audio (Criterion Software), neutral (codfh sub_245670)
 * Source: codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32 (src/rwa/unit_2452A0.c, function sub_245670), a byte-identical
 * decompilation of the Xbox build of RenderWare Audio made with the same compiler (cl 13.00.9466). Used with
 * credit; split into one file per function. Changes: function names (sub_245670 -> rwa_unit2452a0_005c01a0; sub_247290 -> rwa_unit246fd0_005c1f10), the unit's data definitions are
 * extern declarations here, the CODFH_SP (single player) conditionals are resolved for the multiplayer build.
 * Byte-identical to MFL.exe 0x005c01a0 (tools/cmpfn.py, relocations masked). */
#include "rwa.h"
#include "rwaint.h"
extern void rwa_unit246fd0_005c1f10(rwaDecoder *obj, void *objInterface, RwInt32 type, ...);

void
rwa_unit2452a0_005c01a0(rwaDecoder *node, RwInt32 value)
{
    rwa_unit246fd0_005c1f10(node, node->unk28, 2, value);
}
