/* RwaStreamSourceInterfaceRegister (MFL.exe 0x005c87c0): RenderWare Audio (Criterion Software), codfh name
 * Source: codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32 (src/rwa/unit_247430.c, function RwaStreamSourceInterfaceRegister), a byte-identical
 * decompilation of the Xbox build of RenderWare Audio made with the same compiler (cl 13.00.9466). Used with
 * credit; split into one file per function. Changes: function names (sub_243CD0 -> rwa_unit243b40_005bd810; sub_243E20 -> RwaObjInterfaceDefCreate), the unit's data definitions are
 * extern declarations here, the CODFH_SP (single player) conditionals are resolved for the multiplayer build.
 * Byte-identical to MFL.exe 0x005c87c0 (tools/cmpfn.py, relocations masked). */
#include "rwa.h"
#include "rwaint.h"
extern void *RwaObjInterfaceDefCreate(void *iface);
extern void RwaWaveDictAssignID(void *iface, const void *uuid, const void *textID);
extern void rwa_unit243b40_005bd810(void *iface, void *sets, RwInt32 noSets, void *gets, RwInt32 noGets);
extern RwUInt8 dword_5079FC[];      /* the stream source interface */
extern const RwaUUID unk_3ED9C4;
extern void *off_42FBB8[26];
extern void *off_42FC20[26];
extern RwUInt8 dword_5079FC[424];

void *
RwaStreamSourceInterfaceRegister(void)
{
    if (!RwaObjInterfaceDefCreate(dword_5079FC))
    {
        return NULL;
    }

    RwaWaveDictAssignID(dword_5079FC, &unk_3ED9C4, NULL);
    rwa_unit243b40_005bd810(dword_5079FC, off_42FBB8, 5, off_42FC20, 5);

    return dword_5079FC;
}
