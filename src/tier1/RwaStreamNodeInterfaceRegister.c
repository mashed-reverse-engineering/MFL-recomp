/* RwaStreamNodeInterfaceRegister (MFL.exe 0x005c9320): RenderWare Audio (Criterion Software), codfh name
 * Source: codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32 (src/rwa/unit_247430.c, function RwaStreamNodeInterfaceRegister), a byte-identical
 * decompilation of the Xbox build of RenderWare Audio made with the same compiler (cl 13.00.9466). Used with
 * credit; split into one file per function. Changes: function names (sub_243CD0 -> rwa_unit243b40_005bd810; sub_243E20 -> RwaObjInterfaceDefCreate), the unit's data definitions are
 * extern declarations here, the CODFH_SP (single player) conditionals are resolved for the multiplayer build.
 * Byte-identical to MFL.exe 0x005c9320 (tools/cmpfn.py, relocations masked). */
#include "rwa.h"
#include "rwaint.h"
extern void *RwaObjInterfaceDefCreate(void *iface);
extern void RwaWaveDictAssignID(void *iface, const void *uuid, const void *textID);
extern void rwa_unit243b40_005bd810(void *iface, void *sets, RwInt32 noSets, void *gets, RwInt32 noGets);
extern RwUInt8 dword_5079D4[];      /* the stream node interface */
extern const RwUInt8 unk_3ED964[];
extern void *off_42FB54[15];
extern void *off_42FB90[10];
extern RwUInt8 dword_5079D4[40];

void *
RwaStreamNodeInterfaceRegister(void)
{
    if (!RwaObjInterfaceDefCreate(dword_5079D4))
    {
        return NULL;
    }

    RwaWaveDictAssignID(dword_5079D4, unk_3ED964, NULL);
    rwa_unit243b40_005bd810(dword_5079D4, off_42FB54, 3, off_42FB90, 2);

    return dword_5079D4;
}
