/* rwa_param_005be160 (MFL.exe 0x005be160): RenderWare Audio (Criterion Software), neutral (codfh sub_241CD0)
 * Source: codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32 (src/rwa/rwaparam.c, function sub_241CD0), a byte-identical
 * decompilation of the Xbox build of RenderWare Audio made with the same compiler (cl 13.00.9466). Used with
 * credit; split into one file per function. Changes: function names (sub_241CD0 -> rwa_param_005be160), the unit's data definitions are
 * extern declarations here, the CODFH_SP (single player) conditionals are resolved for the multiplayer build.
 * Byte-identical to MFL.exe 0x005be160 (tools/cmpfn.py, relocations masked). */
#include "rwa.h"
#include "rwaint.h"
#define rwaPARAMTYPE_DERIVED    0x4000
extern RwaParamType * sub_241AE0(RwUInt32 type, const void *rangeMin, const void *rangeMax, RwaParamType *paramType, RwUInt8 *rangeStorage);

RwaParamType *
rwa_param_005be160(void *rangeMin, void *rangeMax, RwaParamType *parent, RwaParamType *paramType,
           RwUInt8 *rangeStorage)
{
    RwUInt32        type = parent->type;
    RwaParamType   *result;

    while (!rangeMin && paramType)
    {
        rangeMin = parent->rangeMin;
        paramType = parent->paramType;
    }

    while (!rangeMax && paramType)
    {
        rangeMax = parent->rangeMax;
        paramType = parent->paramType;
    }

    result = sub_241AE0(type, rangeMin, rangeMax, paramType, rangeStorage);
    if (!result)
    {
        return NULL;
    }

    result->paramType = parent;
    result->type |= rwaPARAMTYPE_DERIVED;

    return result;
}
