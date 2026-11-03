/* RwaWaveDictRemoveAllWave (MFL.exe 0x005b8060): RenderWare Audio (Criterion Software), codfh name
 * Source: codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32 (src/rwa/rwawave.c, function RwaWaveDictRemoveAllWave), a byte-identical
 * decompilation of the Xbox build of RenderWare Audio made with the same compiler (cl 13.00.9466). Used with
 * credit; split into one file per function. Changes: function names (sub_243640 -> RwaLListRemoveData), the unit's data definitions are
 * extern declarations here, the CODFH_SP (single player) conditionals are resolved for the multiplayer build.
 * Byte-identical to MFL.exe 0x005b8060 (tools/cmpfn.py, relocations masked). */
#include <string.h>
#include "rwa.h"
#include "rwaint.h"
extern rwaLink dword_516CA8;
#define rwaDictFromLink(_link) ((rwaWaveDict *)((RwUInt8 *)(_link) - 0x1C))
extern void RwaLListRemoveData(rwaLink *list, void *data);

void
RwaWaveDictRemoveAllWave(void *data)
{
    rwaLink    *cur;

    for (cur = dword_516CA8.next; cur != &dword_516CA8; cur = cur->next)
    {
        RwaLListRemoveData(&rwaDictFromLink(cur)->waves, data);
    }
}
