/* rwa_streamsourceobj_005c8840 (MFL.exe 0x005c8840): RenderWare Audio (Criterion Software), neutral (codfh sub_240C70)
 * Source: codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32 (src/rwa/rwastreamsourceobj.c, function sub_240C70), a byte-identical
 * decompilation of the Xbox build of RenderWare Audio made with the same compiler (cl 13.00.9466). Used with
 * credit; split into one file per function. Changes: function names (sub_240C70 -> rwa_streamsourceobj_005c8840; sub_2470A0 -> _rwaStreamNodeProcessComplete; sub_248410 -> __rwaStreamSourceProcess), the unit's data definitions are
 * extern declarations here, the CODFH_SP (single player) conditionals are resolved for the multiplayer build.
 * Byte-identical to MFL.exe 0x005c8840 (tools/cmpfn.py, relocations masked). */
#include "rwa.h"
#include "rwaint.h"
extern void _rwaStreamNodeProcessComplete(rwaDecoder *obj);
extern RwInt32 sub_248520(rwaDecoderState *state);
extern void __rwaStreamSourceProcess(rwaDecoderState *state);

rwaDecoder *
rwa_streamsourceobj_005c8840(rwaDecoder *obj)
{
    rwaDecoderState *state = &obj->state;

    switch (obj->mode)
    {
        case 1:
            if (obj->status != 3)
            {
                _rwaStreamNodeProcessComplete(obj);
            }

            return obj;

        case 2:
            if (obj->status != 6 && obj->status != 7)
            {
                _rwaStreamNodeProcessComplete(obj);
            }

            if (state->flags & 0x10000)
            {
                obj->status = ((state->entries[state->entry].flags & 1) ||
                               (state->entries[state->entry].flags & 4)) ? 7 : 6;
            }
            else
            {
                obj->status = (state->unk154 == state->unk14C &&
                               state->regions[state->unk14C].flags == 1) ? 7 : 6;
            }

            /* fall through */

        case 3:
        {
            RwInt32     result = sub_248520(state);

            if (result == 3 || result == 4)
            {
                if (obj->status == 4)
                {
                    _rwaStreamNodeProcessComplete(obj);
                }

                if (result == 4 && obj->mode == 2)
                {
                    obj->mode = 1;
                    _rwaStreamNodeProcessComplete(obj);
                }
            }

            __rwaStreamSourceProcess(state);

            return obj;
        }
    }

    return obj;
}
