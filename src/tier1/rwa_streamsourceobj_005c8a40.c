/* rwa_streamsourceobj_005c8a40 (MFL.exe 0x005c8a40): RenderWare Audio (Criterion Software), neutral (codfh sub_240E70)
 * Source: codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32 (src/rwa/rwastreamsourceobj.c, function sub_240E70), a byte-identical
 * decompilation of the Xbox build of RenderWare Audio made with the same compiler (cl 13.00.9466). Used with
 * credit; split into one file per function. Changes: function names (sub_240E70 -> rwa_streamsourceobj_005c8a40; sub_247300 -> rwa_unit246fd0_005c1f80; sub_247C50 -> __rwaStreamSourceLockStreamBlock), the unit's data definitions are
 * extern declarations here, the CODFH_SP (single player) conditionals are resolved for the multiplayer build.
 * Byte-identical to MFL.exe 0x005c8a40 (tools/cmpfn.py, relocations masked). */
#include "rwa.h"
#include "rwaint.h"
extern void *__rwaStreamSourceLockStreamBlock(rwaDecoderState *state, rwaStreamRegion **region);
extern rwaQueueItem *rwa_unit246fd0_005c1f80(rwaQueue *queue);
extern RwUInt32 sub_248360(rwaDecoderState *state, rwaStreamRegion *region, RwUInt32 a);

rwaQueueItem *
rwa_streamsourceobj_005c8a40(rwaDecoder *obj)
{
    rwaDecoderState *state = &obj->state;
    rwaStreamRegion *region;
    void           *data;
    rwaQueueItem   *item;

    if (obj->state.queue.count != obj->state.queue.size)
    {
        data = __rwaStreamSourceLockStreamBlock(state, &region);
        if (data)
        {
            item = rwa_unit246fd0_005c1f80(&obj->state.queue);
            item->unk04 = data;
            item->position->unit = 2;
            item->position->value = ((RwUInt32)region->marker - (RwUInt32)state->info->markers) /
                                    sizeof(rwaStreamMarker);
            item->position->pos = sub_248360(state, region, 0);

            if (!state->unkFC && !(state->flags & 0x4000))
            {
                item->flags |= 8;
            }

            return item;
        }
    }

    return NULL;
}
