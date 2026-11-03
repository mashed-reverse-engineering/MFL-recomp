/* _rwaParamTypeHandleRefreshType (MFL.exe 0x005bdf40): RenderWare Audio (Criterion Software), codfh name
 * Source: codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32 (src/rwa/rwaparam.c, function _rwaParamTypeHandleRefreshType), a byte-identical
 * decompilation of the Xbox build of RenderWare Audio made with the same compiler (cl 13.00.9466). Used with
 * credit; split into one file per function. Changes: function names (sub_2446D0 -> rwa_symboltable_005c4590), the unit's data definitions are
 * extern declarations here, the CODFH_SP (single player) conditionals are resolved for the multiplayer build.
 * Byte-identical to MFL.exe 0x005bdf40 (tools/cmpfn.py, relocations masked). */
#include "rwa.h"
#include "rwaint.h"
extern RwUInt8 unk_516C50[];        /* the symbol table */
extern RwaParamType *rwa_symboltable_005c4590(void *table, const RwaUUID *uuid);

RwaParamTypeHandle *
_rwaParamTypeHandleRefreshType(RwaParamTypeHandle *handle)
{
    if (handle->isUUID == TRUE)
    {
        RwaParamType   *paramType = rwa_symboltable_005c4590(unk_516C50, handle->u.paramTypeUUID);

        if (!paramType)
        {
            return NULL;
        }

        handle->u.paramType = paramType;
        handle->isUUID = FALSE;
    }

    return handle;
}
