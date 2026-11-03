/* rwa_objdef_005ba1d0 (MFL.exe 0x005ba1d0): RenderWare Audio (Criterion Software), neutral (codfh sub_23D940)
 * Source: codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32 (src/rwa/rwaobjdef.c, function sub_23D940), a byte-identical
 * decompilation of the Xbox build of RenderWare Audio made with the same compiler (cl 13.00.9466). Used with
 * credit; split into one file per function. Changes: function names (sub_23D940 -> rwa_objdef_005ba1d0), the unit's data definitions are
 * extern declarations here, the CODFH_SP (single player) conditionals are resolved for the multiplayer build.
 * Byte-identical to MFL.exe 0x005ba1d0 (tools/cmpfn.py, relocations masked). */
#include "rwa.h"
#include "rwaint.h"
#include <string.h>
#include <stddef.h>

rwaObjDef *
rwa_objdef_005ba1d0(rwaObjDef *def, void *obj)
{
    rwaLink    *head = &def->children;
    rwaLink    *cur = head->next;

    if (cur != head)
    {
        do
        {
            void   *inst = (RwUInt8 *)cur - 0x18;

            cur = cur->next;
            if (inst == obj)
            {
                return def;
            }
        }
        while (cur != head);
    }

    return NULL;
}
