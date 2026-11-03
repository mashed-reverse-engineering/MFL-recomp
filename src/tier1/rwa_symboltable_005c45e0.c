/* rwa_symboltable_005c45e0 (MFL.exe 0x005c45e0): RenderWare Audio (Criterion Software), neutral (codfh sub_244720)
 * Source: codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32 (src/rwa/rwasymboltable.c, function sub_244720), a byte-identical
 * decompilation of the Xbox build of RenderWare Audio made with the same compiler (cl 13.00.9466). Used with
 * credit; split into one file per function. Changes: function names (sub_244720 -> rwa_symboltable_005c45e0), the unit's data definitions are
 * extern declarations here, the CODFH_SP (single player) conditionals are resolved for the multiplayer build.
 * Byte-identical to MFL.exe 0x005c45e0 (tools/cmpfn.py, relocations masked). */
#include "rwa.h"
#include "rwaint.h"
#define rwaParamTypeFromLink(_link) ((RwaParamType *)((RwUInt8 *)(_link) - 0x20))

RwaParamType *
rwa_symboltable_005c45e0(RwaSymbolTable *table, RwaParamType *parent)
{
    rwaLink        *cur = table->list.next;
    RwaParamType   *paramType = rwaParamTypeFromLink(cur);

    while (paramType->paramType != parent)
    {
        cur = cur->next;
        if (cur == &table->list)
        {
            return NULL;
        }

        paramType = rwaParamTypeFromLink(cur);
    }

    return paramType;
}
