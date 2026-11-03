/* _RwaStreamNodeCreate (MFL.exe 0x005c1c30): RenderWare Audio (Criterion Software), rwa.dll 0.951
 * Source: codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32 (src/rwa/unit_246FD0.c, function sub_246FD0), a byte-identical
 * decompilation of the Xbox build of RenderWare Audio made with the same compiler (cl 13.00.9466). Used with
 * credit; split into one file per function. Changes: function names (sub_243F40 -> rwa_unit243b40_005bda80; sub_246FD0 -> _RwaStreamNodeCreate), the unit's data definitions are
 * extern declarations here, the CODFH_SP (single player) conditionals are resolved for the multiplayer build.
 * Byte-identical to MFL.exe 0x005c1c30 (tools/cmpfn.py, relocations masked). */
#include "rwa.h"
#include "rwaint.h"
extern void *rwa_unit243b40_005bda80(const void *uuid);
extern void *sub_243FF0(void *objInterface, rwaObjDef *def, RwUInt32 a, RwUInt32 b);
extern const RwUInt8 unk_3ED964[];  /* the stream node interface */

/* Sets up a node's shared state from its creation parameters. */
rwaDecoder *
_RwaStreamNodeCreate(rwaDecoder *obj, rwaInstance *parent, rwaStreamNodeParams *params)
{
    rwaStreamNodeState *state = (rwaStreamNodeState *)obj->inst.state;
    RwInt32     value;

    value = params->unk04;
    if (value < 0)
    {
        value = 1;
    }
    state->unk1C = value;
    value = params->unk00;
    if (value < 0)
    {
        value = 1;
    }
    state->unk0C = value;
    state->next = NULL;
    state->unk14 = 0;
    state->unk18 = 0;
    state->numCreated = 0;
    state->callback = params->callback;
    state->mode = 1;
    state->status = 2;
    state->iface = sub_243FF0(rwa_unit243b40_005bda80(unk_3ED964), obj->inst.def, params->unk14, params->unk18);

    return obj;
}
