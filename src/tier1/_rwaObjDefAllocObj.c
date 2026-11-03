/* _rwaObjDefAllocObj (MFL.exe 0x005ba0f0): RenderWare Audio (Criterion Software), rwa.dll 0.952
 * Source: codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32 (src/rwa/rwaobjdef.c, function sub_23D860), a byte-identical
 * decompilation of the Xbox build of RenderWare Audio made with the same compiler (cl 13.00.9466). Used with
 * credit; split into one file per function. Changes: function names (sub_23D860 -> _rwaObjDefAllocObj; sub_243670 -> rwa_unit243670_005bec50), the unit's data definitions are
 * extern declarations here, the CODFH_SP (single player) conditionals are resolved for the multiplayer build.
 * Byte-identical to MFL.exe 0x005ba0f0 (tools/cmpfn.py, relocations masked). */
#include "rwa.h"
#include "rwaint.h"
#include <string.h>
#include <stddef.h>
extern void *rwa_unit243670_005bec50(void *freeList, RwUInt32 hint);
extern void *sub_244440(void *pool, RwUInt32 size);
extern void *sub_242170(RwUInt32 size, RwUInt32 hint);

void *
_rwaObjDefAllocObj(rwaObjDef *def, RwUInt32 size)
{
    if (def->flags0C & 0x40)
    {
        return rwa_unit243670_005bec50(def->instances, def->hint);
    }

    if (def->flags0C & 0x80)
    {
        return sub_244440(def->instances, size);
    }

    return sub_242170(size, def->hint);
}
