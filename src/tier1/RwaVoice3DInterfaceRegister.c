/* RwaVoice3DInterfaceRegister (MFL.exe 0x005bb4b0): RenderWare Audio (Criterion Software), codfh name
 * Source: codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32 (src/rwa/unit_2417C0.c, function RwaVoice3DInterfaceRegister), a byte-identical
 * decompilation of the Xbox build of RenderWare Audio made with the same compiler (cl 13.00.9466). Used with
 * credit; split into one file per function. Changes: function names (sub_243CD0 -> rwa_unit243b40_005bd810; sub_243E20 -> RwaObjInterfaceDefCreate), the unit's data definitions are
 * extern declarations here, the CODFH_SP (single player) conditionals are resolved for the multiplayer build.
 * Byte-identical to MFL.exe 0x005bb4b0 (tools/cmpfn.py, relocations masked). */
#include "rwa.h"
#include "rwaint.h"
extern RwUInt8 unk_506C14[];        /* the voice 3D interface */
extern RwBool RwaObjInterfaceDefCreate(void *iface);
extern void RwaWaveDictAssignID(void *iface, const void *uuid, const void *textID);
extern void rwa_unit243b40_005bd810(void *iface, void *sets, RwInt32 noSets, void *gets, RwInt32 noGets);
extern const RwaUUID unk_3ED3CC;
extern void *unk_42EB08[60];
extern void *unk_42EBF8[66];
extern RwUInt8 unk_506C14[40];

void *
RwaVoice3DInterfaceRegister(void)
{
    if (!RwaObjInterfaceDefCreate(unk_506C14))
    {
        return NULL;
    }

    RwaWaveDictAssignID(unk_506C14, &unk_3ED3CC, NULL);
    rwa_unit243b40_005bd810(unk_506C14, unk_42EB08, 12, unk_42EBF8, 13);

    return unk_506C14;
}
