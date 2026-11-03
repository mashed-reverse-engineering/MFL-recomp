/* rwa_llist_005be5c0 (MFL.exe 0x005be5c0): RenderWare Audio (Criterion Software), neutral (codfh sub_243600)
 * Source: codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32 (src/rwa/rwallist.c, function sub_243600), a byte-identical
 * decompilation of the Xbox build of RenderWare Audio made with the same compiler (cl 13.00.9466). Used with
 * credit; split into one file per function. Changes: function names (sub_243600 -> rwa_llist_005be5c0), the unit's data definitions are
 * extern declarations here, the CODFH_SP (single player) conditionals are resolved for the multiplayer build.
 * Byte-identical to MFL.exe 0x005be5c0 (tools/cmpfn.py, relocations masked). */
#include "rwa.h"
#include "rwaint.h"

RwUInt32
rwa_llist_005be5c0(RwaLLNode *list, void *(*callback)(RwaLLNode *node, void *data), void *data)
{
    RwaLLNode  *node;
    RwUInt32    count = 0;

    for (node = list->prev; node != list; node = node->prev)
    {
        if (callback)
        {
            if (callback(node, data))
            {
                count++;
            }
        }
        else
        {
            count++;
        }
    }

    return count;
}
