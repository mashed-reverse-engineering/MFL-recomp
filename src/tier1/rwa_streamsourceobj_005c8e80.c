/* rwa_streamsourceobj_005c8e80 (MFL.exe 0x005c8e80): RenderWare Audio (Criterion Software), neutral (codfh sub_2412F0)
 * Source: codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32 (src/rwa/rwastreamsourceobj.c, function sub_2412F0), a byte-identical
 * decompilation of the Xbox build of RenderWare Audio made with the same compiler (cl 13.00.9466). Used with
 * credit; split into one file per function. Changes: function names (sub_241230 -> rwa_streamsourceobj_005c8dc0; sub_2412F0 -> rwa_streamsourceobj_005c8e80; sub_243F40 -> rwa_unit243b40_005bda80; sub_246FD0 -> _RwaStreamNodeCreate; sub_2472E0 -> RwaStreamPacketQueueInitialize), the unit's data definitions are
 * extern declarations here, the CODFH_SP (single player) conditionals are resolved for the multiplayer build.
 * Byte-identical to MFL.exe 0x005c8e80 (tools/cmpfn.py, relocations masked). */
#include "rwa.h"
#include "rwaint.h"
extern const RwUInt8 unk_3ED9C4[];
extern rwaQueue *RwaStreamPacketQueueInitialize(rwaQueue *queue, rwaQueueItem *items, RwUInt8 count);
extern RwBool _RwaStreamNodeCreate(rwaDecoder *obj, rwaInstance *parent, rwaStreamNodeParams *params,
                         void *a, void *b);
extern void rwa_streamsourceobj_005c8dc0(rwaDecoderState *state, RwBool failed, rwaDecoder *obj);
extern RwBool sub_249160(rwaDecoderState *state, rwaStreamSourceParams *params);
extern void *rwa_unit243b40_005bda80(const void *uuid);
extern void *sub_243FF0(void *objInterface, rwaObjDef *def, RwUInt32 a, RwUInt32 b);
extern void rwa_streamsourceobj_005c8dc0(rwaDecoderState *state, RwBool failed, rwaDecoder *arg);

rwaDecoder *
rwa_streamsourceobj_005c8e80(rwaDecoder *obj, rwaInstance *parent, rwaStreamNodeParams *params, void *a, void *b)
{
    rwaStreamSourceParams   source;

    source.values[0] = params->source[0];
    source.values[6] = params->source[6];
    source.values[7] = params->source[7];
    source.values[2] = params->source[2];
    source.values[4] = params->source[4];
    source.values[5] = params->source[5];
    source.values[3] = params->source[3];
    source.values[1] = params->source[1];
    source.values[14] = params->source[14];
    source.values[13] = params->source[13];
    source.values[12] = params->source[12];
    source.values[8] = params->source[8];
    source.values[9] = params->source[9];
    source.data = obj;
    source.callback = rwa_streamsourceobj_005c8dc0;
    source.values[11] = params->source[11];
    source.values[10] = params->source[10];

    RwaStreamPacketQueueInitialize(&obj->state.queue, NULL, 0);

    if (obj->inst.subObjLink.next)
    {
        obj->inst.subObjLink.prev->next = obj->inst.subObjLink.next;
        obj->inst.subObjLink.next->prev = obj->inst.subObjLink.prev;
        obj->inst.subObjLink.prev = NULL;
        obj->inst.subObjLink.next = NULL;
    }

    if (!_RwaStreamNodeCreate(obj, parent, params, a, b))
    {
        return NULL;
    }

    if (!sub_249160(&obj->state, &source))
    {
        return NULL;
    }

    obj->objInterface = sub_243FF0(rwa_unit243b40_005bda80(unk_3ED9C4), obj->inst.def, params->unk14,
                                   params->unk18);

    return obj;
}
