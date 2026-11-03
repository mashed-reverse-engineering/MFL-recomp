/* rwa_symboltable_005c4510 (MFL.exe 0x005c4510): RenderWare Audio (Criterion Software), neutral (codfh sub_244650)
 * Source: codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32 (src/rwa/rwasymboltable.c, function sub_244650), a byte-identical
 * decompilation of the Xbox build of RenderWare Audio made with the same compiler (cl 13.00.9466). Used with
 * credit; split into one file per function. Changes: function names (sub_244650 -> rwa_symboltable_005c4510), the unit's data definitions are
 * extern declarations here, the CODFH_SP (single player) conditionals are resolved for the multiplayer build.
 * Byte-identical to MFL.exe 0x005c4510 (tools/cmpfn.py, relocations masked). */
#include "rwa.h"
#include "rwaint.h"

/* Return types: rwacore_ps2.h declares both symbol-table mutations as
 * RwaSymbolTable*. The image leaves table in EAX at 0x244682/0x2446C6. */
RwaSymbolTable *
rwa_symboltable_005c4510(RwaSymbolTable *table, RwaParamType *paramType)
{
    paramType->link.prev = NULL;
    paramType->link.next = NULL;
    paramType->link.next = table->list.next;
    paramType->link.prev = &table->list;
    table->list.next->prev = &paramType->link;
    table->list.next = &paramType->link;
    table->noParamTypes++;
    return table;
}
