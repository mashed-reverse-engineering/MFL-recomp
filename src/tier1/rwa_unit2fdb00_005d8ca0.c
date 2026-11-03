/* rwa_unit2fdb00_005d8ca0 (MFL.exe 0x005d8ca0): RenderWare Audio (Criterion Software), neutral (codfh sub_2FEC00)
 * Source: codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32 (src/rwa/unit_2FDB00.c, function sub_2FEC00), a byte-identical
 * decompilation of the Xbox build of RenderWare Audio made with the same compiler (cl 13.00.9466). Used with
 * credit; split into one file per function. Changes: function names (sub_23F960 -> RwaObjGetParamUInt32; sub_2FEC00 -> rwa_unit2fdb00_005d8ca0), the unit's data definitions are
 * extern declarations here, the CODFH_SP (single player) conditionals are resolved for the multiplayer build.
 * Byte-identical to MFL.exe 0x005d8ca0 (tools/cmpfn.py, relocations masked). */
#include "rwa.h"
#include "rwaint.h"
extern RwInt32 RwaObjGetParamUInt32(RwaObjHandle *handle, RwInt32 param, RwInt32 type, RwInt32 value);
extern RwaObjHandle **dword_5126B4;
extern RwUInt32 dword_5126B8;

/* The first of them not muted (0: all are). */
void *
rwa_unit2fdb00_005d8ca0(void *obj, const void *param, RwaObjHandle **value)
{
    RwUInt32    i;

    for (i = 0; i < dword_5126B8; i++)
    {
        if (!RwaObjGetParamUInt32(dword_5126B4[i], 6, 2, 0))
        {
            *value = dword_5126B4[i];
            return obj;
        }
    }
    *value = NULL;

    return obj;
}
