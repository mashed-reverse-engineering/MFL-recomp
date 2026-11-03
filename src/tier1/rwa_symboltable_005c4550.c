/* rwa_symboltable_005c4550 (MFL.exe 0x005c4550): RenderWare Audio (Criterion Software), neutral (codfh sub_244690)
 * Source: codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32 (src/rwa/rwasymboltable.c, function sub_244690), a byte-identical
 * decompilation of the Xbox build of RenderWare Audio made with the same compiler (cl 13.00.9466). Used with
 * credit; split into one file per function. Changes: function names (sub_244690 -> rwa_symboltable_005c4550), the unit's data definitions are
 * extern declarations here, the CODFH_SP (single player) conditionals are resolved for the multiplayer build.
 * Byte-identical to MFL.exe 0x005c4550 (tools/cmpfn.py, relocations masked). */
#include "rwa.h"
#include "rwaint.h"
#define rwaParamTypeFromLink(_link) ((RwaParamType *)((RwUInt8 *)(_link) - 0x20))

RwaSymbolTable *
rwa_symboltable_005c4550(RwaSymbolTable *table, RwaParamType *paramType)
{
    rwaLink    *cur;

    for (cur = table->list.next; cur != &table->list; cur = cur->next)
    {
        if (rwaParamTypeFromLink(cur) == paramType)
        {
            break;
        }
    }

    table->noParamTypes--;
    paramType->link.prev->next = paramType->link.next;
    paramType->link.next->prev = paramType->link.prev;
    return table;
}
