/* RwaParamTypeCreateArray and its static helpers: RenderWare Audio (Criterion Software)
 * Source: codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32 (src/rwa/rwaparam.c, functions sub_241960 (inlined, static),
 * sub_241980 (0x005bdde0, static), sub_241AE0 (0x005bdf70)), a byte-identical decompilation of the Xbox build of RenderWare Audio. Used with
 * credit; these functions stay in one file because cl compiles a static function and its callers in the same file with a register
 * calling convention (sub_241980 takes paramType in esi). Changes: function names (sub_241920 -> RwaAtomicFree; sub_241980 ->
 * rwa_param_005bdde0; sub_241AE0 -> RwaParamTypeCreateArray, the MFL symbol name), the unit's data definitions are extern declarations here. */
#include "rwa.h"
#include "rwaint.h"
extern RwUInt8 unk_506F64[];        /* parameter types (a static of the original unit) */
extern RwUInt8 unk_516C50[];        /* the symbol table */
extern RwUInt32 sub_23E440(RwUInt32 type);
extern void *sub_243670(void *freeList, RwUInt32 hint);
extern void sub_2437A0(void *freeList, void *item);
extern RwaSymbolTable *sub_244650(RwaSymbolTable *table, RwaParamType *type);
extern RwaUniqueID *sub_240B40(RwaUniqueID *uniqueID);
extern void *sub_2418E0(RwUInt32 type);
extern void RwaAtomicFree(void *value, RwUInt32 type);
extern void *memcpy(void *dst, const void *src, RwUInt32 size);

static void
sub_241960(RwaParamType *paramType)
{
    if (!(paramType->flags & 1))
    {
        sub_2437A0(unk_506F64, paramType);
    }
}

/* MFL.exe 0x005bdde0 */
static void
rwa_param_005bdde0(RwaParamType *paramType)
{
    if (!(paramType->flags & 2))
    {
        if (paramType->rangeMin)
        {
            RwaAtomicFree(paramType->rangeMin, paramType->type);
        }

        if (paramType->rangeMax)
        {
            RwaAtomicFree(paramType->rangeMax, paramType->type);
        }
    }
}

/* MFL.exe 0x005bdf70 */
RwaParamType *
RwaParamTypeCreateArray(RwUInt32 type, const void *rangeMin, const void *rangeMax, RwaParamType *paramType,
                        RwUInt8 *rangeStorage)
{
    RwUInt32    size = sub_23E440(type);

    if (!paramType)
    {
        paramType = (RwaParamType *)sub_243670(unk_506F64, rwaMEMHINTDUR_EVENT | rwaID_803);
        paramType->flags = 0;
    }
    else
    {
        paramType->flags = 1;
    }

    paramType->rangeMin = NULL;
    paramType->rangeMax = NULL;

    if (rangeStorage)
    {
        paramType->flags |= 2;

        if (rangeMin)
        {
            paramType->rangeMin = rangeStorage;
            if (rangeMax)
            {
                paramType->rangeMax = rangeStorage + sub_23E440(type);
            }
        }
        else if (rangeMax)
        {
            paramType->rangeMax = rangeStorage;
        }
    }
    else
    {
        if (rangeMin)
        {
            paramType->rangeMin = sub_2418E0(type);
            if (!paramType->rangeMin)
            {
                rwa_param_005bdde0(paramType);
            }
        }

        if (rangeMax)
        {
            paramType->rangeMax = sub_2418E0(type);
            if (!paramType->rangeMax)
            {
                sub_241960(paramType);
                rwa_param_005bdde0(paramType);
            }
        }
    }

    if (rangeMin)
    {
        memcpy(paramType->rangeMin, rangeMin, size);
    }

    if (rangeMax)
    {
        memcpy(paramType->rangeMax, rangeMax, size);
    }

    sub_240B40(&paramType->uniqueID);
    paramType->type = type;
    paramType->paramType = NULL;
    sub_244650((RwaSymbolTable *)unk_516C50, paramType);

    return paramType;
}
