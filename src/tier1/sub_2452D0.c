/* sub_2452D0 (MFL.exe 0x005bfdc0): RenderWare Audio (Criterion Software), 
 * Source: codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32 (src/rwa/unit_2452A0.c, function sub_2452D0), a byte-identical
 * decompilation of the Xbox build of RenderWare Audio made with the same compiler (cl 13.00.9466; MFL.exe was built with cl 13.10). Used with
 * credit; split into one file per function. Changes: function names (none), the unit's data definitions are
 * extern declarations here, the CODFH_SP (single player) conditionals are resolved for the multiplayer build.
 * Byte-identical to MFL.exe 0x005bfdc0 (tools/cmpfn.py, relocations masked). */
/* compile: cl1310 */
#include "rwa2.h"
#include "rwaint2.h"
extern RwInt32 RwaUUIDCompare(const RwaUUID *uuid1, const RwaUUID *uuid2);
extern const RwaUUID unk_3EDEBC;    /* the format converter's definition */

/* Walks each node chain up to the output node or a format converter, calling
 * func on the nodes before it; returns the last chain's stop node (the walk
 * stops after the first chain without a func). */
rwaDecoder *
sub_2452D0(rwaStreamPlayer *player, rwaContainerNodeFunc func, void *data)
{
    rwaLink    *link;
    rwaDecoder *found;

    found = NULL;
    link = player->nodes.next;
    while (link != &player->nodes)
    {
            rwaDecoder *node = rwaDecoderFromLink(link);

            link = link->next;
            do
            {
                rwaDecoder *next = node->next;
                rwaObjDef  *def = node->inst.def;

                if (player->decoder == node
                    || !RwaUUIDCompare(def->uniqueID.uuid, &unk_3EDEBC))
                {
                    found = node;
                    break;
                }
                if (func)
                {
                    func(node, data);
                }
                node = next;
            }
            while (node);
        if (!func)
        {
            break;
        }
    }

    return found;
}
