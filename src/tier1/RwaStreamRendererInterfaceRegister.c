/* RwaStreamRendererInterfaceRegister (MFL.exe 0x005c92c0): RenderWare Audio (Criterion Software), codfh name
 * Source: codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32 (src/rwa/unit_24CC50.c, function RwaStreamRendererInterfaceRegister), a byte-identical
 * decompilation of the Xbox build of RenderWare Audio made with the same compiler (cl 13.00.9466). Used with
 * credit; split into one file per function. Changes: function names (sub_243CD0 -> rwa_unit243b40_005bd810; sub_243E20 -> RwaObjInterfaceDefCreate), the unit's data definitions are
 * extern declarations here, the CODFH_SP (single player) conditionals are resolved for the multiplayer build.
 * Byte-identical to MFL.exe 0x005c92c0 (tools/cmpfn.py, relocations masked). */
#include "rwa.h"
#include "rwaint.h"
extern void *RwaObjInterfaceDefCreate(void *iface);
extern void RwaWaveDictAssignID(void *iface, const void *uuid, const void *textID);
extern void rwa_unit243b40_005bd810(void *iface, void *sets, RwInt32 noSets, void *gets, RwInt32 noGets);
extern RwUInt8 dword_507C4C[];      /* the stream renderer interface */
extern const RwUInt8 unk_3EDEFC[];  /* the stream renderer interface */
extern void *off_430090[26];
extern void *off_4300F8[106];
extern RwUInt8 dword_507C4C[44];

void *
RwaStreamRendererInterfaceRegister(void)
{
    if (!RwaObjInterfaceDefCreate(dword_507C4C))
    {
        return NULL;
    }

    RwaWaveDictAssignID(dword_507C4C, unk_3EDEFC, NULL);
    rwa_unit243b40_005bd810(dword_507C4C, off_430090, 5, off_4300F8, 4);

    return dword_507C4C;
}
