/* RwaStreamNodeCreationComplete (MFL.exe 0x005c1cb0): RenderWare Audio (Criterion Software), codfh name
 * Source: codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32 (src/rwa/unit_246FD0.c, function RwaStreamNodeCreationComplete), a byte-identical
 * decompilation of the Xbox build of RenderWare Audio made with the same compiler (cl 13.00.9466). Used with
 * credit; split into one file per function. Changes: function names (none), the unit's data definitions are
 * extern declarations here, the CODFH_SP (single player) conditionals are resolved for the multiplayer build.
 * Byte-identical to MFL.exe 0x005c1cb0 (tools/cmpfn.py, relocations masked). */
#include "rwa.h"
#include "rwaint.h"

/* A node has been created: its status from its mode, then the callback. */
void
RwaStreamNodeCreationComplete(rwaDecoder *obj)
{
    rwaStreamNodeState *state = (rwaStreamNodeState *)obj->inst.state;

    if (state->status != 1)
    {
        switch (state->mode)
        {
        case 1:
            state->status = 3;
            break;
        case 2:
            state->status = 6;
            break;
        case 3:
            state->status = 4;
            break;
        }
        state->numCreated++;
    }

    if (state->callback)
    {
        state->callback->func((rwaQueueItem *)obj, state->callback->data);
    }
}
