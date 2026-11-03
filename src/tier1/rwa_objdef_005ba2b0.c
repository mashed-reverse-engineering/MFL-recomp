/* rwa_objdef_005ba2b0 (MFL.exe 0x005ba2b0): RenderWare Audio (Criterion Software), neutral (codfh sub_23DA20)
 * Source: codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32 (src/rwa/rwaobjdef.c, function sub_23DA20), a byte-identical
 * decompilation of the Xbox build of RenderWare Audio made with the same compiler (cl 13.00.9466). Used with
 * credit; split into one file per function. Changes: function names (sub_23DA20 -> rwa_objdef_005ba2b0; sub_2434E0 -> RwaLListAddData), the unit's data definitions are
 * extern declarations here, the CODFH_SP (single player) conditionals are resolved for the multiplayer build.
 * Byte-identical to MFL.exe 0x005ba2b0 (tools/cmpfn.py, relocations masked). */
#include "rwa.h"
#include "rwaint.h"
#include <string.h>
#include <stddef.h>
extern RwBool sub_243BC0(void *iface, rwaObjDef *def, RwInt32 index);
extern RwBool RwaLListAddData(void *list, void *item);
extern void sub_2435D0(void *list);

RwInt32
rwa_objdef_005ba2b0(rwaObjDef *parent, rwaObjDef *child, rwaIfaceSearch *search)
{
    if ((child->flags0C & 4) && search->list &&
        sub_243BC0(search->iface, child, 0) && !RwaLListAddData(search->list, child))
    {
        sub_2435D0(search->list);
        search->list = NULL;
    }

    return 0;
}
