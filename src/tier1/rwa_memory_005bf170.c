/* rwa_memory_005bf170 (MFL.exe 0x005bf170): RenderWare Audio (Criterion Software), neutral (codfh fnxCalloc)
 * Source: codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32 (src/rwa/rwamemory.c, function _fnxCalloc), a byte-identical
 * decompilation of the Xbox build of RenderWare Audio made with the same compiler (cl 13.00.9466). Used with
 * credit; split into one file per function. Changes: function names (_fnxCalloc -> rwa_memory_005bf170), the unit's data definitions are
 * extern declarations here, the CODFH_SP (single player) conditionals are resolved for the multiplayer build.
 * Byte-identical to MFL.exe 0x005bf170 (tools/cmpfn.py, relocations masked). */
#include "rwa.h"
#include "rwaint.h"
extern void *malloc(RwUInt32 size);
extern void *memset(void *dst, RwInt32 value, RwUInt32 size);

void *
rwa_memory_005bf170(RwUInt32 count, RwUInt32 size, RwUInt32 hint)
{
    RwUInt32    total = count * size;
    void       *mem = malloc(total);

    if (mem)
    {
        memset(mem, 0, total);
    }

    return mem;
}
