/* rwa_obj_005bace0 (MFL.exe 0x005bace0): RenderWare Audio (Criterion Software), neutral (codfh sub_23F140)
 * Source: codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32 (src/rwa/rwaobj.c, function sub_23F140), a byte-identical
 * decompilation of the Xbox build of RenderWare Audio made with the same compiler (cl 13.00.9466). Used with
 * credit; split into one file per function. Changes: function names (sub_23F140 -> rwa_obj_005bace0), the unit's data definitions are
 * extern declarations here, the CODFH_SP (single player) conditionals are resolved for the multiplayer build.
 * Byte-identical to MFL.exe 0x005bace0 (tools/cmpfn.py, relocations masked). */
#include "rwa.h"
#include "rwaint.h"
#define rwaInstFromSubLink(_link) ((rwaInstance *)((RwUInt8 *)(_link) - 0x14))

rwaInstance *
rwa_obj_005bace0(rwaInstance *volatile input, RwBool ok)
{
    /* Preserve the parameter-home reloads at the two fallback paths.
     * Their source qualifier is unknown; this local qualification reproduces
     * the observed reads without qualifying the shared instance type. */
    rwaInstance    *obj = input;
    rwaInstance    *result = obj;
    rwaLink        *cur;
    rwaInstance    *status;

    if (!ok)
    {
        if (obj->def->unk30)
        {
            status = obj->def->unk30(obj);
        }
        else
        {
            status = input;
        }
        if (!status)
        {
            result = NULL;
        }
    }
    else
    {
        status = input;
    }

    cur = obj->subObjs.next;
    while (cur != &obj->subObjs)
    {
        rwaInstance    *sub = rwaInstFromSubLink(cur);

        cur = cur->next;
        if (sub->def->unk30)
        {
            status = sub->def->unk30(sub);
        }

        if (!status)
        {
            result = NULL;
        }
    }

    return result;
}
