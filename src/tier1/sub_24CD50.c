/* sub_24CD50 (MFL.exe 0x005c8100): RenderWare Audio (Criterion Software), 
 * Source: codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32 (src/rwa/unit_24CC50.c, function sub_24CD50), a byte-identical
 * decompilation of the Xbox build of RenderWare Audio made with the same compiler (cl 13.00.9466; MFL.exe was built with cl 13.10). Used with
 * credit; split into one file per function. Changes: function names (sub_247200 -> RwaStreamConnectionGetPacket), the unit's data definitions are
 * extern declarations here, the CODFH_SP (single player) conditionals are resolved for the multiplayer build.
 * Byte-identical to MFL.exe 0x005c8100 (tools/cmpfn.py, relocations masked). */
/* compile: cl1310 */
#include "rwa2.h"
#include "rwaint2.h"
extern rwaQueueItem *RwaStreamConnectionGetPacket(rwaStreamReader *reader);
extern rwaQueueItem *sub_24CD50(rwaSplitter *obj, RwUInt32 track);

rwaQueueItem *
sub_24CD50(rwaSplitter *obj, RwUInt32 track)
{
    RwUInt32   *index;
    rwaQueueItem *item;

    if (obj->state.node.status == 3)
    {
        return NULL;
    }

    index = &obj->state.readIndex[track];
    if (!obj->state.items[*index])
    {
        obj->state.items[*index] = RwaStreamConnectionGetPacket((rwaStreamReader *)&obj->state.node.next);
        if (obj->state.items[*index])
        {
            obj->state.masks[*index] = 0xFFFFFFFF;
            obj->state.starved = FALSE;
        }
        else
        {
            obj->state.starved = TRUE;
            return NULL;
        }
    }
    if (obj->state.masks[*index] & (1 << track))
    {
        item = &obj->state.trackItems[*index + track * obj->state.noBuffers];
        item->flags = obj->state.items[*index]->flags;
        item->position = obj->state.items[*index]->position;
        item->unk08 = obj->state.info.tracks[track].unk20;
        item->unk04 = obj->state.items[*index]->unk04 + obj->state.info.tracks[track].unk24;
        if (++*index >= obj->state.noBuffers)
        {
            *index = 0;
        }

        return item;
    }
    return NULL;
}
