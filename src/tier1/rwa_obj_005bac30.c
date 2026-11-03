/* rwa_obj_005bac30 (MFL.exe 0x005bac30): RenderWare Audio (Criterion Software), neutral (codfh sub_23F090)
 * Source: codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32 (src/rwa/rwaobj.c, function sub_23F090), a byte-identical
 * decompilation of the Xbox build of RenderWare Audio made with the same compiler (cl 13.00.9466). Used with
 * credit; split into one file per function. Changes: function names (sub_23F090 -> rwa_obj_005bac30), the unit's data definitions are
 * extern declarations here, the CODFH_SP (single player) conditionals are resolved for the multiplayer build.
 * Byte-identical to MFL.exe 0x005bac30 (tools/cmpfn.py, relocations masked). */
#include "rwa.h"
#include "rwaint.h"
#define rwaInstFromLink(_link) ((rwaInstance *)((RwUInt8 *)(_link) - 4))
extern rwaLink dword_506A28;

void
rwa_obj_005bac30(RwBool (*callback)(rwaInstance *obj, void *data), void *data)
{
    rwaLink    *cur;

    for (cur = dword_506A28.next; cur != &dword_506A28; cur = cur->next)
    {
        if (!callback(rwaInstFromLink(cur), data))
        {
            break;
        }
    }
}
