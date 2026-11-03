/* RwaAtomicAlloc (MFL.exe 0x005bdd60): RenderWare Audio (Criterion Software), rwa.dll 0.973
 * Source: codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32 (src/rwa/rwaparam.c, function sub_2418E0), a byte-identical
 * decompilation of the Xbox build of RenderWare Audio made with the same compiler (cl 13.00.9466). Used with
 * credit; split into one file per function. Changes: function names (sub_23E440 -> rwa_base_005b6730; sub_2418E0 -> RwaAtomicAlloc; sub_243670 -> rwa_unit243670_005bec50), the unit's data definitions are
 * extern declarations here, the CODFH_SP (single player) conditionals are resolved for the multiplayer build.
 * Byte-identical to MFL.exe 0x005bdd60 (tools/cmpfn.py, relocations masked). */
#include "rwa.h"
#include "rwaint.h"
#define rwaNOVALUEFREELISTS     5
extern rwaValueFreeList unk_42EF98[rwaNOVALUEFREELISTS];
extern RwUInt32 rwa_base_005b6730(RwUInt32 type);
extern void *rwa_unit243670_005bec50(void *freeList, RwUInt32 hint);
extern rwaValueFreeList unk_42EF98[5];

// End of its data definitions.
void *
RwaAtomicAlloc(RwUInt32 type)
{
    RwUInt32    size = rwa_base_005b6730(type);
    RwUInt32    i;

    for (i = 0; i < rwaNOVALUEFREELISTS; i++)
    {
        if (unk_42EF98[i].size == size)
        {
            return rwa_unit243670_005bec50(unk_42EF98[i].freeList, rwaMEMHINTDUR_EVENT | rwaID_803);
        }
    }

    return NULL;
}
