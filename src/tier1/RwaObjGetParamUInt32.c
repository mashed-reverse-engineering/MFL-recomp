/* RwaObjGetParamUInt32 (MFL.exe 0x005b7370): RenderWare Audio (Criterion Software), rwa.dll 1.0
 * Source: codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32 (src/rwa/rwaobject.c, function sub_23F960), a byte-identical
 * decompilation of the Xbox build of RenderWare Audio made with the same compiler (cl 13.00.9466). Used with
 * credit; split into one file per function. Changes: function names (sub_23F8E0 -> rwa_object_005b72d0; sub_23F960 -> RwaObjGetParamUInt32), the unit's data definitions are
 * extern declarations here, the CODFH_SP (single player) conditionals are resolved for the multiplayer build.
 * Byte-identical to MFL.exe 0x005b7370 (tools/cmpfn.py, relocations masked). */
#include "rwa.h"
#include "rwaint.h"
extern RwaObjHandle *rwa_object_005b72d0(RwaObjHandle *handle, RwUInt32 index, RwInt32 type, void *data);
extern RwaObjHandle * rwa_object_005b72d0(RwaObjHandle *handle, RwUInt32 index, RwInt32 type, void *data);

RwUInt32
RwaObjGetParamUInt32(RwaObjHandle *handle, RwUInt32 index, RwInt32 type, RwBool *success)
{
    RwaObjHandle   *result = rwa_object_005b72d0(handle, index, type, &index);

    if (success)
    {
        *success = result != NULL;
    }

    return index;
}
