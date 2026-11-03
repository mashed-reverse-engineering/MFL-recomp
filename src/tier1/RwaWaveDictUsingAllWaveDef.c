/* RwaWaveDictUsingAllWaveDef (MFL.exe 0x005b7f70): RenderWare Audio (Criterion Software), codfh name
 * Source: codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32 (src/rwa/rwawave.c, function RwaWaveDictUsingAllWaveDef), a byte-identical
 * decompilation of the Xbox build of RenderWare Audio made with the same compiler (cl 13.00.9466). Used with
 * credit; split into one file per function. Changes: function names (none), the unit's data definitions are
 * extern declarations here, the CODFH_SP (single player) conditionals are resolved for the multiplayer build.
 * Byte-identical to MFL.exe 0x005b7f70 (tools/cmpfn.py, relocations masked). */
#include <string.h>
#include "rwa.h"
#include "rwaint.h"
extern rwaLink dword_516CA8;
#define rwaDictFromLink(_link) ((rwaWaveDict *)((RwUInt8 *)(_link) - 0x1C))

RwUInt32
RwaWaveDictUsingAllWaveDef(RwUInt32 id)
{
    rwaLink    *cur;

    for (cur = dword_516CA8.next; cur != &dword_516CA8; cur = cur->next)
    {
        rwaWaveDict    *dict = rwaDictFromLink(cur);
        rwaWaveNode    *node;

        for (node = (rwaWaveNode *)dict->waves.prev;
             node != (rwaWaveNode *)&dict->waves;
             node = node->next)
        {
            rwaWaveItem    *item = (rwaWaveItem *)node->item;

            if (item->id == id)
            {
                return id;
            }
        }
    }

    return 0;
}
