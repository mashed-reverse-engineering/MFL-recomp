/* _rwaStreamContainerEnumSourceChain (MFL.exe 0x005bfe40): RenderWare Audio (Criterion Software), rwa.dll 0.919
 * Source: codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32 (src/rwa/unit_2452A0.c, function sub_245350), a byte-identical
 * decompilation of the Xbox build of RenderWare Audio made with the same compiler (cl 13.00.9466). Used with
 * credit; split into one file per function. Changes: function names (sub_245350 -> _rwaStreamContainerEnumSourceChain), the unit's data definitions are
 * extern declarations here, the CODFH_SP (single player) conditionals are resolved for the multiplayer build.
 * Byte-identical to MFL.exe 0x005bfe40 (tools/cmpfn.py, relocations masked). */
#include "rwa.h"
#include "rwaint.h"

/* Calls func on node and the nodes chained after it. */
void
_rwaStreamContainerEnumSourceChain(rwaDecoder *node, rwaContainerNodeFunc func, void *data)
{
    do
    {
        rwaDecoder *next = node->next;

        func(node, data);
        node = next;
    }
    while (node);
}
