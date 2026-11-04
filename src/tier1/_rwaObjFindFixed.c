/* _rwaObjFindFixed and its static helpers: RenderWare Audio (Criterion Software)
 * Source: codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32 (src/rwa/rwaobj.c, functions sub_23EF50 (0x005bab10, static), _rwaObjFindFixed (0x005baf50)), a byte-identical
 * decompilation of the Xbox build of RenderWare Audio made with the same compiler (cl 13.00.9466; MFL.exe was built with cl 13.10). Used with
 * credit; these functions stay in one file because cl 13.00 compiles a static function and its callers in the same
 * file with a register calling convention. Changes: function names (sub_23EF50 -> rwa_obj_005bab10; sub_23F090 -> rwa_obj_005bac30), the unit's data definitions are
 * extern declarations here, the CODFH_SP (single player) conditionals are resolved for the multiplayer build.
 * Byte-identical to MFL.exe (tools/cmpfn.py, relocations masked): rwa_obj_005bab10 0x005bab10, _rwaObjFindFixed 0x005baf50. Static functions keep their linkage: the
 * overlay replaces only the external ones, the static copies serve the callers in this file. */
/* compile: cl1310 */
#include "rwa.h"
#include "rwaint.h"
extern void rwa_obj_005bac30(RwBool (*callback)(rwaInstance *obj, void *data), void *data);

/* MFL.exe 0x005bab10 */
static RwBool
rwa_obj_005bab10(rwaInstance *obj, rwaInstSearch *search)
{
    if (obj->def == search->def && (obj->flags & 4))
    {
        search->found = obj->flags;

        return FALSE;
    }

    return TRUE;
}

/* MFL.exe 0x005baf50 */
RwUInt32
_rwaObjFindFixed(rwaObjDef *def)
{
    rwaInstSearch   search;

    search.def = def;
    search.found = 0;
    rwa_obj_005bac30((RwBool (*)(rwaInstance *, void *))rwa_obj_005bab10, &search);

    return search.found;
}
