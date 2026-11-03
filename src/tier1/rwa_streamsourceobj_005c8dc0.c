/* rwa_streamsourceobj_005c8dc0 (MFL.exe 0x005c8dc0): RenderWare Audio (Criterion Software), neutral (codfh sub_241230)
 * Source: codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32 (src/rwa/rwastreamsourceobj.c, function sub_241230), a byte-identical
 * decompilation of the Xbox build of RenderWare Audio made with the same compiler (cl 13.00.9466). Used with
 * credit; split into one file per function. Changes: function names (sub_240E20 -> rwa_streamsourceobj_005c89f0; sub_241230 -> rwa_streamsourceobj_005c8dc0; sub_2472E0 -> RwaStreamPacketQueueInitialize), the unit's data definitions are
 * extern declarations here, the CODFH_SP (single player) conditionals are resolved for the multiplayer build.
 * Byte-identical to MFL.exe 0x005c8dc0 (tools/cmpfn.py, relocations masked). */
#include "rwa.h"
#include "rwaint.h"
extern void RwaStreamNodeCreationComplete(rwaDecoder *obj);
extern rwaQueue *RwaStreamPacketQueueInitialize(rwaQueue *queue, rwaQueueItem *items, RwUInt8 count);
extern void rwa_streamsourceobj_005c8dc0(rwaDecoderState *state, RwBool failed, rwaDecoder *obj);
extern void rwa_streamsourceobj_005c89f0(rwaQueueItem *item, rwaDecoder *obj);

void
rwa_streamsourceobj_005c8dc0(rwaDecoderState *state, RwBool failed, rwaDecoder *arg)
{
    rwaDecoder *obj = arg;
    RwUInt32 count;
    rwaQueueItem *items;
    rwaStreamPosition *positions;
    RwUInt32 i;
    rwaQueueCallback *callback;

    if (failed)
    {
        obj->status = 1;
    }
    else
    {
        positions = obj->state.positions;
        count = obj->state.noItems;
        items = obj->state.items;

        callback = &obj->state.callback;
        callback->func = rwa_streamsourceobj_005c89f0;
        callback->data = obj;

        for (i = 0; i < count; i++)
        {
            items[i].flags = 0;
            items[i].unk04 = (RwUInt8 *)obj;
            items[i].callback = callback;
            items[i].unk08 = state->info->unk20;
            items[i].position = &positions[i];
            positions[i].type = 3;
            positions[i].unit = 2;
        }

        RwaStreamPacketQueueInitialize(&obj->state.queue, items, count);
    }

    RwaStreamNodeCreationComplete(obj);
}
