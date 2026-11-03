/* _rwaObjDefSetupParams (MFL.exe 0x005ba620): RenderWare Audio (Criterion Software), codfh name
 * Source: codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32 (src/rwa/rwaobjdef.c, function _rwaObjDefSetupParams), a byte-identical
 * decompilation of the Xbox build of RenderWare Audio made with the same compiler (cl 13.00.9466). Used with
 * credit; split into one file per function. Changes: function names (none), the unit's data definitions are
 * extern declarations here, the CODFH_SP (single player) conditionals are resolved for the multiplayer build.
 * Byte-identical to MFL.exe 0x005ba620 (tools/cmpfn.py, relocations masked). */
#include "rwa.h"
#include "rwaint.h"
#include <string.h>
#include <stddef.h>
extern RwaParamTypeHandle *_rwaParamTypeHandleRefreshType(RwaParamTypeHandle *handle);
extern void *TkFSManagerGetValue(void *obj, RwUInt32 index, void *data);
extern void *sub_242170(RwUInt32 size, RwUInt32 hint);

/* The allocation and existing-table paths share the success return. */
rwaObjDef *
_rwaObjDefSetupParams(rwaObjDef *def, rwaParamDef *inputs, RwUInt16 noInputs,
                      rwaParamDef *outputs, RwUInt16 noOutputs)
{
    RwUInt16    i;

    if (!inputs && !outputs)
    {
        def->flags0C &= ~1;
        ((rwaParamTables *)def->interfaces)->inputs =
            sub_242170((noInputs + noOutputs) * sizeof(rwaParamDef), rwaMEMHINTDUR_EVENT | rwaID_809);
        if (!((rwaParamTables *)def->interfaces)->inputs)
        {
            return NULL;
        }

        ((rwaParamTables *)def->interfaces)->outputs =
            ((rwaParamTables *)def->interfaces)->inputs + noInputs;
    }
    else
    {
        for (i = 0; i < noInputs; i++)
        {
            rwaParamDef    *param = &inputs[i];

            if (!_rwaParamTypeHandleRefreshType((RwaParamTypeHandle *)&param->type))
            {
                return NULL;
            }

            if (!param->handler)
            {
                param->handler = TkFSManagerGetValue;
            }
        }

        ((rwaParamTables *)def->interfaces)->inputs = inputs;
        ((rwaParamTables *)def->interfaces)->noInputs = noInputs;

        for (i = 0; i < noOutputs; i++)
        {
            rwaParamDef    *param = &outputs[i];

            if (!_rwaParamTypeHandleRefreshType((RwaParamTypeHandle *)&param->type))
            {
                return NULL;
            }

            if (!param->handler)
            {
                param->handler = TkFSManagerGetValue;
            }
        }

        def->flags0C |= 1;
        ((rwaParamTables *)def->interfaces)->outputs = outputs;
        ((rwaParamTables *)def->interfaces)->noOutputs = noOutputs;

    }
    return def;
}
