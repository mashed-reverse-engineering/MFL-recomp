/* rwa_symboltable_005c44f0 (MFL.exe 0x005c44f0): RenderWare Audio (Criterion Software), neutral (codfh sub_244630)
 * Source: codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32 (src/rwa/rwasymboltable.c, function sub_244630), a byte-identical
 * decompilation of the Xbox build of RenderWare Audio made with the same compiler (cl 13.00.9466). Used with
 * credit; split into one file per function. Changes: function names (sub_244630 -> rwa_symboltable_005c44f0), the unit's data definitions are
 * extern declarations here, the CODFH_SP (single player) conditionals are resolved for the multiplayer build.
 * Byte-identical to MFL.exe 0x005c44f0 (tools/cmpfn.py, relocations masked). */
#include "rwa.h"
#include "rwaint.h"

void
rwa_symboltable_005c44f0(RwaSymbolTable *table)
{
    table->noParamTypes = 0;
    table->list.next = &table->list;
    table->list.prev = &table->list;
}
