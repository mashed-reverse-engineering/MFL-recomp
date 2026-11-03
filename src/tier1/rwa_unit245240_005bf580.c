/* rwa_unit245240_005bf580 (MFL.exe 0x005bf580): RenderWare Audio (Criterion Software), neutral (codfh sub_245240)
 * Source: codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32 (src/rwa/unit_245240.c, function sub_245240), a byte-identical
 * decompilation of the Xbox build of RenderWare Audio made with the same compiler (cl 13.00.9466). Used with
 * credit; split into one file per function. Changes: function names (sub_243CD0 -> rwa_unit243b40_005bd810; sub_243E20 -> RwaObjInterfaceDefCreate; sub_245240 -> rwa_unit245240_005bf580), the unit's data definitions are
 * extern declarations here, the CODFH_SP (single player) conditionals are resolved for the multiplayer build.
 * Byte-identical to MFL.exe 0x005bf580 (tools/cmpfn.py, relocations masked). */
#include "rwa.h"
#include "rwaint.h"
extern RwUInt8 unk_5079AC[];        /* the interface */
extern RwBool RwaObjInterfaceDefCreate(void *iface);
extern void RwaWaveDictAssignID(void *iface, const void *uuid, const void *textID);
extern void rwa_unit243b40_005bd810(void *iface, void *sets, RwInt32 noSets, void *gets, RwInt32 noGets);
extern const RwaUUID unk_3ED864;
extern void *unk_42FA10[30];
extern void *unk_42FA88[45];
extern RwUInt8 unk_5079AC[40];

// End of its data definitions.
void *
rwa_unit245240_005bf580(void)
{
    if (!RwaObjInterfaceDefCreate(unk_5079AC))
    {
        return NULL;
    }

    RwaWaveDictAssignID(unk_5079AC, &unk_3ED864, NULL);
    rwa_unit243b40_005bd810(unk_5079AC, unk_42FA10, 6, unk_42FA88, 9);

    return unk_5079AC;
}
