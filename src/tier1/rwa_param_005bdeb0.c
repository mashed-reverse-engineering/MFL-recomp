/* rwa_param_005bdeb0 (MFL.exe 0x005bdeb0): RenderWare Audio (Criterion Software), neutral (codfh sub_241A20)
 * Source: codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32 (src/rwa/rwaparam.c, function sub_241A20), a byte-identical
 * decompilation of the Xbox build of RenderWare Audio made with the same compiler (cl 13.00.9466). Used with
 * credit; split into one file per function. Changes: function names (sub_241A20 -> rwa_param_005bdeb0; sub_2438E0 -> rwa_unit243670_005beec0; sub_244630 -> rwa_symboltable_005c44f0), the unit's data definitions are
 * extern declarations here, the CODFH_SP (single player) conditionals are resolved for the multiplayer build.
 * Byte-identical to MFL.exe 0x005bdeb0 (tools/cmpfn.py, relocations masked). */
#include "rwa.h"
#include "rwaint.h"
#define rwaNOVALUEFREELISTS     5
extern RwUInt32 dword_506F58;
extern RwUInt8 unk_506F64[0x24];
extern RwUInt32 dword_506FD0;
extern rwaValueFreeList unk_42EF98[rwaNOVALUEFREELISTS];
extern RwUInt8 unk_516C50[];        /* the symbol table */
extern void rwa_unit243670_005beec0(void *freeList);
extern void rwa_symboltable_005c44f0(void *table);
extern RwaParamType *sub_244750(void *table, RwaParamType *parent);
extern rwaValueFreeList unk_42EF98[5];
extern void sub_2419B0(RwaParamType *paramType, RwBool recurse);

void
rwa_param_005bdeb0(void)
{
    RwaParamType       *paramType;
    RwUInt32            i;

    dword_506F58 = 1;

    while ((paramType = sub_244750(unk_516C50, NULL)) != NULL)
    {
        sub_2419B0(paramType, TRUE);
    }

    rwa_symboltable_005c44f0(unk_516C50);
    rwa_unit243670_005beec0(unk_506F64);

    for (i = rwaNOVALUEFREELISTS; i > 0; i--)
    {
        rwa_unit243670_005beec0(unk_42EF98[i - 1].freeList);
    }

    dword_506FD0 = 0;
}
