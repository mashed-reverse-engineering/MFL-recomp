/* RwaCoreOpen and its static helpers: RenderWare Audio (Criterion Software)
 * Source: codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32 (src/rwa/rwabase.c, functions sub_23E240 (0x005b6580, static),
 * sub_23E420 (inlined), RwaCoreOpen (0x005b6800)), a byte-identical decompilation of the Xbox build of RenderWare Audio. Used with
 * credit; these functions stay in one file because cl compiles a static function and its callers in the same file with a register
 * calling convention (sub_23E240 takes openCallBack in ebx). Changes: function names (sub_23E240 -> rwa_base_005b6580), the unit's
 * data definitions are extern declarations here, MFL's PC build opens 16 modules (codfh Xbox: 17), sub_23E420 is a local copy
 * (the original unit inlines it; its own definition stays original).
 * P7c: byte-identical to MFL.exe with cl 13.00 and cl 13.10 (tools/cmpfn.py). */
#include "rwa.h"
#include "rwaint.h"
typedef struct RwMemoryFunctions RwMemoryFunctions;
extern RwMemoryFunctions *memFuncs;
extern RwUInt32 dword_5069BC;
extern RwUInt32 dword_5069B4;
extern RwUInt32 dword_5069B8;
extern RwUInt32 dword_5069A8;
extern RwUInt32 dword_516C5C;
extern rwaModule unk_42E628[];
extern RwBool sub_23E2E0(rwaModule *module, RwUInt32 index);
extern void sub_23E320(rwaModule *module, RwUInt32 index);

/* MFL.exe 0x005b6580 */
static RwBool
rwa_base_005b6580(rwaModule *modules, RwUInt32 count, rwaModuleCallBack closeCallBack, rwaModuleCallBack openCallBack)
{
    RwUInt32    i;
    RwBool      result;

    for (i = 0; i < count;)
    {
        result = modules[i].open();
        if (!result)
        {
            goto failed;
        }

        if (openCallBack)
        {
            result = openCallBack(&modules[i], i);
        }

        i++;
        if (!result)
        {
            goto failed;
        }
    }

    return TRUE;

  failed:

    while (i > 0)
    {
        modules[i - 1].close();
        if (closeCallBack)
        {
            closeCallBack(&modules[i - 1], i - 1);
        }

        i--;
    }

    return FALSE;
}

static RwUInt32
rwaCoreSetAlignment(RwUInt32 size)
{
    if (size < 0x10)
    {
        dword_516C5C = 0x10;

        return 0x10;
    }

    dword_516C5C = size;

    return size;
}

/* MFL.exe 0x005b6800 */
RwBool
RwaCoreOpen(RwMemoryFunctions *memoryFunctions, RwBool usingRwGraphics)
{
    rwaCoreSetAlignment(0x10);
    dword_5069BC = usingRwGraphics;
    memFuncs = memoryFunctions;

    if (rwa_base_005b6580(unk_42E628, 16, (rwaModuleCallBack)sub_23E320, (rwaModuleCallBack)sub_23E2E0) == TRUE)
    {
        dword_5069B8 = 0;
        dword_5069B4 = 0;
        dword_5069A8 = 1;
        return TRUE;
    }
    return FALSE;
}
