/* RwaObjInterfaceRemove (MFL.exe 0x005bd6c0): RenderWare Audio (Criterion Software), rwa.dll 0.93
 * Source: codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32 (src/rwa/unit_243B40.c, function sub_243B80), a byte-identical
 * decompilation of the Xbox build of RenderWare Audio made with the same compiler (cl 13.00.9466). Used with
 * credit; split into one file per function. Changes: function names (sub_243B80 -> RwaObjInterfaceRemove), the unit's data definitions are
 * extern declarations here, the CODFH_SP (single player) conditionals are resolved for the multiplayer build.
 * Byte-identical to MFL.exe 0x005bd6c0 (tools/cmpfn.py, relocations masked). */
#include "rwa.h"
#include "rwaint.h"

RwBool
RwaObjInterfaceRemove(rwaObjInterface *objInterface)
{
    if (objInterface->refCount > 0)
    {
        objInterface->refCount--;

        return FALSE;
    }

    objInterface->link.prev->next = objInterface->link.next;
    objInterface->link.next->prev = objInterface->link.prev;

    if (!(objInterface->flags & 1))
    {
        sub_2421B0(objInterface);
    }

    return TRUE;
}
