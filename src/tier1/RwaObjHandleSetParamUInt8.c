/* RwaObjHandleSetParamUInt8 (MFL.exe 0x005b7300): RenderWare Audio (Criterion Software), codfh name
 * Source: codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32 (src/rwa/rwaobject.c, function RwaObjHandleSetParamUInt8), a byte-identical
 * decompilation of the Xbox build of RenderWare Audio made with the same compiler (cl 13.00.9466). Used with
 * credit; split into one file per function. Changes: function names (Behavior_DispatchCallback -> rwa_object_005b72a0), the unit's data definitions are
 * extern declarations here, the CODFH_SP (single player) conditionals are resolved for the multiplayer build.
 * Byte-identical to MFL.exe 0x005b7300 (tools/cmpfn.py, relocations masked). */
#include "rwa.h"
#include "rwaint.h"
extern RwaObjHandle *rwa_object_005b72a0(RwaObjHandle *handle, RwUInt32 index, RwInt32 type,
                                               void *data);
extern RwaObjHandle * rwa_object_005b72a0(RwaObjHandle *handle, RwUInt32 index, RwInt32 type, void *data);

RwaObjHandle *
RwaObjHandleSetParamUInt8(RwaObjHandle *handle, RwUInt32 index, RwInt32 type, ...)
{
    if (!handle)
    {
        return handle;
    }

    return rwa_object_005b72a0(handle, index, type, &type + 1);
}
