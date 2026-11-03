/* rwa_unit243b40_005bd810 (MFL.exe 0x005bd810): RenderWare Audio (Criterion Software), neutral (codfh sub_243CD0)
 * Source: codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32 (src/rwa/unit_243B40.c, function sub_243CD0), a byte-identical
 * decompilation of the Xbox build of RenderWare Audio made with the same compiler (cl 13.00.9466). Used with
 * credit; split into one file per function. Changes: function names (sub_243CD0 -> rwa_unit243b40_005bd810), the unit's data definitions are
 * extern declarations here, the CODFH_SP (single player) conditionals are resolved for the multiplayer build.
 * Byte-identical to MFL.exe 0x005bd810 (tools/cmpfn.py, relocations masked). */
#include "rwa.h"
#include "rwaint.h"

RwaObjInterfaceDef *
rwa_unit243b40_005bd810(RwaObjInterfaceDef *def, _rwaParamID *inputs, RwUInt32 noInputs,
           _rwaParamID *outputs, RwUInt32 noOutputs)
{
    if (def->inputs && def->outputs)
    {
        if (!(def->flags & 1))
        {
            sub_2421B0(def->inputs);
        }

        def->inputs = NULL;
        def->outputs = NULL;
    }

    if (inputs && outputs)
    {
        RwUInt32    i;

        for (i = 0; i < noInputs; i++)
        {
            inputs[i].uniqueID.flags = 0;
            if (!_rwaParamTypeHandleRefreshType(&inputs[i].type))
            {
                return NULL;
            }
        }

        for (i = 0; i < noOutputs; i++)
        {
            outputs[i].uniqueID.flags = 0;
            if (!_rwaParamTypeHandleRefreshType(&outputs[i].type))
            {
                return NULL;
            }
        }

        def->inputs = inputs;
        def->flags |= 1;
        def->noOutputs = (RwUInt16)noOutputs;
        def->outputs = outputs;
        def->noInputs = (RwUInt16)noInputs;

        return def;
    }
    else
    {
        RwUInt32    size = (noInputs + noOutputs) * sizeof(_rwaParamID);
        _rwaParamID *ids = (_rwaParamID *)sub_242170(size, rwaMEMHINTDUR_EVENT | rwaID_80A);

        def->inputs = ids;
        if (!ids)
        {
            return NULL;
        }

        def->outputs = &ids[noInputs];
        memset(ids, 0, size);
        def->flags &= ~1;

        return def;
    }
}
