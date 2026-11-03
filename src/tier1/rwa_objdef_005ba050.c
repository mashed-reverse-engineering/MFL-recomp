/* rwa_objdef_005ba050 (MFL.exe 0x005ba050): RenderWare Audio (Criterion Software), neutral (codfh sub_23D7C0)
 * Source: codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32 (src/rwa/rwaobjdef.c, function sub_23D7C0), a byte-identical
 * decompilation of the Xbox build of RenderWare Audio made with the same compiler (cl 13.00.9466). Used with
 * credit; split into one file per function. Changes: function names (sub_23D7C0 -> rwa_objdef_005ba050; sub_243950 -> rwa_unit243670_005bef40), the unit's data definitions are
 * extern declarations here, the CODFH_SP (single player) conditionals are resolved for the multiplayer build.
 * Byte-identical to MFL.exe 0x005ba050 (tools/cmpfn.py, relocations masked). */
#include "rwa.h"
#include "rwaint.h"
#include <string.h>
#include <stddef.h>
extern void *rwa_unit243670_005bef40(RwUInt32 size, RwUInt32 count, RwUInt32 align, RwInt32 a, void *freeList, RwUInt32 hint);
extern void *sub_244550(RwUInt32 size, RwUInt32 count, RwUInt32 align, RwInt32 a, RwInt32 b);

rwaObjDef *
rwa_objdef_005ba050(rwaObjDef *def, RwUInt32 size, RwBool freeList)
{
    if (def->instances)
    {
        return NULL;
    }

    if (def->maxInstances > 1)
    {
        if (freeList)
        {
            def->instances = rwa_unit243670_005bef40(size, def->maxInstances, 1 << (def->flags >> 28), 1, NULL,
                                        rwaMEMHINTDUR_EVENT | rwaID_809);
            def->flags0C |= 0x40;
        }
        else
        {
            def->instances = sub_244550(size, def->maxInstances, 1 << (def->flags >> 28), 1, 0);
            def->flags0C |= 0x80;
        }
    }
    else
    {
        def->instances = (void *)1;
    }

    return def->instances ? def : NULL;
}
