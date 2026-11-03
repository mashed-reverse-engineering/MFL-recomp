/* rwa_unit246fd0_005c1fc0 (MFL.exe 0x005c1fc0): RenderWare Audio (Criterion Software), neutral (codfh sub_247340)
 * Source: codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32 (src/rwa/unit_246FD0.c, function sub_247340), a byte-identical
 * decompilation of the Xbox build of RenderWare Audio made with the same compiler (cl 13.00.9466). Used with
 * credit; split into one file per function. Changes: function names (sub_247340 -> rwa_unit246fd0_005c1fc0), the unit's data definitions are
 * extern declarations here, the CODFH_SP (single player) conditionals are resolved for the multiplayer build.
 * Byte-identical to MFL.exe 0x005c1fc0 (tools/cmpfn.py, relocations masked). */
#include "rwa.h"
#include "rwaint.h"

/* Drops the first item. */
void
rwa_unit246fd0_005c1fc0(rwaQueue *queue)
{
    RwUInt32    head = queue->head + 1;

    if (head >= queue->size)
    {
        head -= queue->size;
    }
    queue->count--;
    queue->head = (RwUInt8)head;
}
