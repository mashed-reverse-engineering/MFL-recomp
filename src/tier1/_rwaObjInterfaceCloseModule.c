/* _rwaObjInterfaceCloseModule (MFL.exe 0x005bdad0): RenderWare Audio (Criterion Software), rwa.dll 0.971
 * Source: codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32 (src/rwa/unit_243B40.c, function sub_243F90), a byte-identical
 * decompilation of the Xbox build of RenderWare Audio made with the same compiler (cl 13.00.9466). Used with
 * credit; split into one file per function. Changes: function names (sub_2438E0 -> rwa_unit243670_005beec0; sub_243EA0 -> rwa_unit243b40_005bd9e0; sub_243F90 -> _rwaObjInterfaceCloseModule), the unit's data definitions are
 * extern declarations here, the CODFH_SP (single player) conditionals are resolved for the multiplayer build.
 * Byte-identical to MFL.exe 0x005bdad0 (tools/cmpfn.py, relocations masked). */
#include "rwa.h"
#include "rwaint.h"
#define rwaInterfaceDefFromLink(_link) ((RwaObjInterfaceDef *)((RwUInt8 *)(_link) - 0x20))
extern RwUInt8 unk_50708C[0x24];
extern rwaLink dword_5070B0;
extern RwUInt32 dword_5070B8;
extern RwBool rwa_unit243b40_005bd9e0(RwaObjInterfaceDef *def);

void
_rwaObjInterfaceCloseModule(void)
{
    while (dword_5070B0.next != &dword_5070B0)
    {
        rwa_unit243b40_005bd9e0(rwaInterfaceDefFromLink(dword_5070B0.next));
    }

    dword_5070B0.next = &dword_5070B0;
    dword_5070B0.prev = &dword_5070B0;
    rwa_unit243670_005beec0(unk_50708C);
    dword_5070B8 = 0;
}
