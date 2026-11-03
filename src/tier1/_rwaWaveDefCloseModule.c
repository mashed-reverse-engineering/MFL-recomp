/* _rwaWaveDefCloseModule (MFL.exe 0x005bd650): RenderWare Audio (Criterion Software), codfh name
 * Source: codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32 (src/rwa/unit_242910.c, function _rwaWaveDefCloseModule), a byte-identical
 * decompilation of the Xbox build of RenderWare Audio made with the same compiler (cl 13.00.9466). Used with
 * credit; split into one file per function. Changes: function names (sub_23D970 -> rwa_objdef_005ba200; sub_2438E0 -> rwa_unit243670_005beec0), the unit's data definitions are
 * extern declarations here, the CODFH_SP (single player) conditionals are resolved for the multiplayer build.
 * Byte-identical to MFL.exe 0x005bd650 (tools/cmpfn.py, relocations masked). */
#include "rwa.h"
#include "rwaint.h"
extern void rwa_objdef_005ba200(void *a, void *b,
                       RwBool (*callback)(rwaObjDef *parent, rwaObjDef *objDef, void *data),
                       void *data, RwBool recurse);
extern RwUInt8 unk_507004[0x24];
extern RwUInt32 dword_507028;
extern RwBool sub_243060(rwaObjDef *parent, rwaObjDef *objDef);

void
_rwaWaveDefCloseModule(void)
{
    rwa_objdef_005ba200(NULL, NULL, (RwBool (*)(rwaObjDef *, rwaObjDef *, void *))sub_243060, NULL,
               TRUE);
    rwa_unit243670_005beec0(unk_507004);
    dword_507028 = 0;
}
