/* rwa_unit244220_005be970 (MFL.exe 0x005be970): RenderWare Audio (Criterion Software), neutral (codfh sub_244360)
 * Source: codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32 (src/rwa/unit_244220.c, function sub_244360), a byte-identical
 * decompilation of the Xbox build of RenderWare Audio made with the same compiler (cl 13.00.9466). Used with
 * credit; split into one file per function. Changes: function names (sub_244360 -> rwa_unit244220_005be970), the unit's data definitions are
 * extern declarations here, the CODFH_SP (single player) conditionals are resolved for the multiplayer build.
 * Byte-identical to MFL.exe 0x005be970 (tools/cmpfn.py, relocations masked). */
#include "rwa.h"
#include "rwaint.h"

void
rwa_unit244220_005be970(rwaPoolBlock *block, void *data)
{
    rwaPoolChunk   *chunk = block->chunks;

    do
    {
        if (chunk + 1 == (rwaPoolChunk *)data)
        {
            if (chunk->prev != block->end)
            {
                chunk->prev->next = chunk->next;
            }

            if (chunk->next != block->end)
            {
                chunk->next->prev = chunk->prev;
            }

            return;
        }

        chunk = chunk->next;
    }
    while (chunk != block->end);
}
