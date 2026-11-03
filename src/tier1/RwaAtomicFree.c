/* RwaAtomicFree (MFL.exe 0x005bdda0): RenderWare Audio (Criterion Software), rwa.dll 0.97
 * Source: codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32 (src/rwa/rwaparam.c, function sub_241920), a byte-identical
 * decompilation of the Xbox build of RenderWare Audio made with the same compiler (cl 13.00.9466). Used with
 * credit; split into one file per function. Changes: function names (sub_23E440 -> rwa_base_005b6730; sub_241920 -> RwaAtomicFree; sub_2437A0 -> rwa_unit243670_005bed80), the unit's data definitions are
 * extern declarations here, the CODFH_SP (single player) conditionals are resolved for the multiplayer build.
 * Byte-identical to MFL.exe 0x005bdda0 (tools/cmpfn.py, relocations masked). */
#include "rwa.h"
#include "rwaint.h"
#define rwaNOVALUEFREELISTS     5
extern rwaValueFreeList unk_42EF98[rwaNOVALUEFREELISTS];
extern RwUInt32 rwa_base_005b6730(RwUInt32 type);
extern void rwa_unit243670_005bed80(void *freeList, void *item);
extern rwaValueFreeList unk_42EF98[5];

void
RwaAtomicFree(void *value, RwUInt32 type)
{
    RwUInt32    size = rwa_base_005b6730(type);
    RwUInt32    i;

    for (i = 0; i < rwaNOVALUEFREELISTS; i++)
    {
        if (unk_42EF98[i].size == size)
        {
            rwa_unit243670_005bed80(unk_42EF98[i].freeList, value);

            return;
        }
    }
}
