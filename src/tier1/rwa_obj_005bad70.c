/* rwa_obj_005bad70 and its static helpers: RenderWare Audio (Criterion Software)
 * Source: codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32 (src/rwa/rwaobj.c, functions sub_23F1B0 (0x005bad50, static), sub_23F1D0 (0x005bad70)), a byte-identical
 * decompilation of the Xbox build of RenderWare Audio made with the same compiler (cl 13.00.9466). Used with
 * credit; these functions stay in one file because cl 13.00 compiles a static function and its callers in the same
 * file with a register calling convention. Changes: function names (sub_23F1B0 -> rwa_obj_005bad50; sub_23F1D0 -> rwa_obj_005bad70; sub_243600 -> rwa_llist_005be5c0), the unit's data definitions are
 * extern declarations here, the CODFH_SP (single player) conditionals are resolved for the multiplayer build.
 * Byte-identical to MFL.exe (tools/cmpfn.py, relocations masked): rwa_obj_005bad50 0x005bad50, rwa_obj_005bad70 0x005bad70. Static functions keep their linkage: the
 * overlay replaces only the external ones, the static copies serve the callers in this file. */
#include "rwa.h"
#include "rwaint.h"
#define rwaInstFromLink(_link) ((rwaInstance *)((RwUInt8 *)(_link) - 4))
extern rwaLink dword_506A28;
extern void *rwa_llist_005be5c0(rwaLink *list, rwaLink *(*callback)(rwaLink *, void *), void *data);

/* MFL.exe 0x005bad50 */
static rwaLink *
rwa_obj_005bad50(rwaLink *link, void *def)
{
    return rwaInstFromLink(link)->def == def ? link : NULL;
}

/* MFL.exe 0x005bad70 */
void *
rwa_obj_005bad70(rwaObjDef *def)
{
    return rwa_llist_005be5c0(&dword_506A28, rwa_obj_005bad50, def);
}
