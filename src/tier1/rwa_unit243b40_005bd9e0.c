/* rwa_unit243b40_005bd9e0 (MFL.exe 0x005bd9e0): RenderWare Audio (Criterion Software), neutral (codfh sub_243EA0)
 * Source: codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32 (src/rwa/unit_243B40.c, function sub_243EA0), a byte-identical
 * decompilation of the Xbox build of RenderWare Audio made with the same compiler (cl 13.00.9466). Used with
 * credit; split into one file per function. Changes: function names (sub_2437A0 -> rwa_unit243670_005bed80; sub_243EA0 -> rwa_unit243b40_005bd9e0), the unit's data definitions are
 * extern declarations here, the CODFH_SP (single player) conditionals are resolved for the multiplayer build.
 * Byte-identical to MFL.exe 0x005bd9e0 (tools/cmpfn.py, relocations masked). */
#include "rwa.h"
#include "rwaint.h"
extern RwUInt8 unk_50708C[0x24];

RwBool
rwa_unit243b40_005bd9e0(RwaObjInterfaceDef *def)
{
    RwUInt32    count;
    RwUInt32    i;

    if (def->unregister)
    {
        def->unregister(def);
    }

    sub_240C30(&def->uniqueID);

    count = def->noInputs;
    for (i = 0; i < count; i++)
    {
        sub_240C30(&def->inputs[i].uniqueID);
    }

    count = def->noOutputs;
    for (i = 0; i < count; i++)
    {
        sub_240C30(&def->outputs[i].uniqueID);
    }

    if (!(def->flags & 1))
    {
        sub_2421B0(def->inputs);
    }

    def->link.prev->next = def->link.next;
    def->link.next->prev = def->link.prev;

    if (!(def->flags & 2))
    {
        rwa_unit243670_005bed80(unk_50708C, def);
    }

    return TRUE;
}
