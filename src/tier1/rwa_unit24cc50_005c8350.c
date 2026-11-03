/* rwa_unit24cc50_005c8350 (MFL.exe 0x005c8350): RenderWare Audio (Criterion Software), neutral (codfh sub_24CFF0)
 * Source: codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32 (src/rwa/unit_24CC50.c, function sub_24CFF0), a byte-identical
 * decompilation of the Xbox build of RenderWare Audio made with the same compiler (cl 13.00.9466). Used with
 * credit; split into one file per function. Changes: function names (sub_243F40 -> rwa_unit243b40_005bda80; sub_246FD0 -> _RwaStreamNodeCreate; sub_24CEF0 -> rwa_unit24cc50_005c8220; sub_24CFF0 -> rwa_unit24cc50_005c8350), the unit's data definitions are
 * extern declarations here, the CODFH_SP (single player) conditionals are resolved for the multiplayer build.
 * Byte-identical to MFL.exe 0x005c8350 (tools/cmpfn.py, relocations masked). */
#include "rwa.h"
#include "rwaint.h"
extern void *rwa_unit243b40_005bda80(const void *uuid);
extern void *sub_243FF0(void *objInterface, rwaObjDef *def, RwUInt32 a, RwUInt32 b);
extern rwaDecoder *_RwaStreamNodeCreate(rwaDecoder *obj, rwaInstance *parent, rwaStreamNodeParams *params,
                              void *a, void *b);
extern void RwaStreamNodeCreationComplete(rwaDecoder *obj);
extern const RwUInt8 unk_3ED964[];  /* the stream node interface */
extern rwaSplitter *rwa_unit24cc50_005c8350(rwaSplitter *obj, rwaInstance *parent, rwaSplitterParams *params,
                               void *a, void *b);
extern void rwa_unit24cc50_005c8220(rwaQueueItem *item, rwaSplitterTrack *track);
static __forceinline void *
splitAlloc(rwaNodeAllocator *allocator, RwUInt32 size)
{
    return allocator->alloc(size, allocator->data);
}

rwaSplitter *
rwa_unit24cc50_005c8350(rwaSplitter *obj, rwaInstance *parent, rwaSplitterParams *params, void *a, void *b)
{
    rwaSplitterState *state = &obj->state;
    RwUInt32    noTracks;
    RwUInt32    i;
    RwUInt32    j;
    rwaQueueCallback *callbacks;

    if (!_RwaStreamNodeCreate((rwaDecoder *)obj, parent, (rwaStreamNodeParams *)params, a, b))
    {
        return NULL;
    }

    obj->state.objInterface = sub_243FF0(rwa_unit243b40_005bda80(unk_3ED964), obj->inst.def, 0, 0);
    noTracks = params->info->noTracks;
    state->allocator = params->allocator;
    state->info = *params->info;
    state->noBuffers = params->noBuffers;
    state->masks = (RwUInt32 *)splitAlloc(state->allocator, noTracks * (state->noBuffers * sizeof(rwaQueueItem) + 24) + state->noBuffers * 24);
    if (!state->masks)
    {
        return NULL;
    }

    state->items = (rwaQueueItem **)(state->masks + state->noBuffers);
    state->readIndex = (RwUInt32 *)(state->items + state->noBuffers);
    state->writeIndex = state->readIndex + noTracks;
    state->trackItems = (rwaQueueItem *)(state->writeIndex + noTracks);
    callbacks = (rwaQueueCallback *)(state->trackItems + state->noBuffers * noTracks);
    state->tracks = (rwaSplitterTrack *)(callbacks + noTracks);
    state->callbacks = callbacks;
    for (i = 0; i < state->noBuffers; i++)
    {
        state->items[i] = NULL;
        state->masks[i] = 0;
    }
    for (i = 0; i < params->info->noTracks; i++)
    {
        state->readIndex[i] = 0;
        state->writeIndex[i] = 0;
        state->tracks[i].owner = state;
        /* Evidence: 0x24D124 reloads tracks before the index store at
         * 0x24D12A. This qualified access reproduces its address encoding;
         * it does not establish an original volatile declaration. */
        ((volatile rwaSplitterState *)state)->tracks[i].index = i;
        state->callbacks[i].func = (void (*)(rwaQueueItem *, rwaDecoder *))rwa_unit24cc50_005c8220;
        state->callbacks[i].data = (rwaDecoder *)&state->tracks[i];
        for (j = 0; j < state->noBuffers; j++)
        {
            state->trackItems[state->noBuffers * i + j].callback = &state->callbacks[i];
        }
    }
    state->starved = FALSE;
    RwaStreamNodeCreationComplete((rwaDecoder *)obj);

    return obj;
}
