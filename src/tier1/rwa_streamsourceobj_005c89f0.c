/* rwa_streamsourceobj_005c89f0 (MFL.exe 0x005c89f0): RenderWare Audio (Criterion Software), neutral (codfh sub_240E20)
 * Source: codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32 (src/rwa/rwastreamsourceobj.c, function sub_240E20), a byte-identical
 * decompilation of the Xbox build of RenderWare Audio made with the same compiler (cl 13.00.9466). Used with
 * credit; split into one file per function. Changes: function names (sub_240E20 -> rwa_streamsourceobj_005c89f0; sub_247160 -> rwa_unit246fd0_005c1dc0; sub_247340 -> rwa_unit246fd0_005c1fc0), the unit's data definitions are
 * extern declarations here, the CODFH_SP (single player) conditionals are resolved for the multiplayer build.
 * Byte-identical to MFL.exe 0x005c89f0 (tools/cmpfn.py, relocations masked). */
#include "rwa.h"
#include "rwaint.h"
extern void rwa_unit246fd0_005c1dc0(rwaDecoder *obj, RwUInt32 a, RwUInt32 *value);
extern void sub_247E00(rwaDecoderState *state, void *a);
extern void rwa_unit246fd0_005c1fc0(rwaQueue *queue, rwaQueueItem *item);

void
rwa_streamsourceobj_005c89f0(rwaQueueItem *item, rwaDecoder *obj)
{
    if (item->flags & 8)
    {
        RwUInt32    value;

        item->flags &= ~8;
        value = 1;
        rwa_unit246fd0_005c1dc0(obj, 0, &value);
    }

    sub_247E00(&obj->state, item->unk04);
    rwa_unit246fd0_005c1fc0(&obj->state.queue, item);
}
