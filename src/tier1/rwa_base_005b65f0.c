/* rwa_base_005b65f0 (MFL.exe 0x005b65f0): RenderWare Audio (Criterion Software), neutral (codfh sub_23E2E0)
 * Source: codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32 (src/rwa/rwabase.c, function sub_23E2E0), a byte-identical
 * decompilation of the Xbox build of RenderWare Audio made with the same compiler (cl 13.00.9466). Used with
 * credit; split into one file per function. Changes: function names (sub_23E2E0 -> rwa_base_005b65f0), the unit's data definitions are
 * extern declarations here, the CODFH_SP (single player) conditionals are resolved for the multiplayer build.
 * Byte-identical to MFL.exe 0x005b65f0 (tools/cmpfn.py, relocations masked). */
#include "rwa.h"
#include "rwaint.h"
typedef struct RwMemoryFunctions RwMemoryFunctions;
extern RwMemoryFunctions *memFuncs;
extern RwBool sub_23E1C0(void);
extern RwMemoryFunctions *RwOsGetMemoryInterface(void);
extern RwInt32 sub_242240(RwMemoryFunctions *memFuncs);

RwBool
rwa_base_005b65f0(rwaModule *module, RwUInt32 index)
{
    if (module->open == sub_23E1C0)
    {
        if (!memFuncs)
        {
            memFuncs = RwOsGetMemoryInterface();
        }

        return sub_242240(memFuncs) == 1;
    }

    return TRUE;
}
