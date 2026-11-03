/* rwa_llist_005be530 (MFL.exe 0x005be530): RenderWare Audio (Criterion Software), neutral (codfh sub_243540)
 * Source: codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32 (src/rwa/rwallist.c, function sub_243540), a byte-identical
 * decompilation of the Xbox build of RenderWare Audio made with the same compiler (cl 13.00.9466). Used with
 * credit; split into one file per function. Changes: function names (sub_243540 -> rwa_llist_005be530), the unit's data definitions are
 * extern declarations here, the CODFH_SP (single player) conditionals are resolved for the multiplayer build.
 * Byte-identical to MFL.exe 0x005be530 (tools/cmpfn.py, relocations masked). */
#include "rwa.h"
#include "rwaint.h"

RwInt32
rwa_llist_005be530(RwaLLNode *list, void *data)
{
    RwaLLNode  *node;
    RwInt32     index = 0;

    for (node = list->next; node != list; node = node->next)
    {
        if (node->data == data)
        {
            return index;
        }

        index++;
    }

    return -1;
}
