/* RwaObjDefSetInterface (MFL.exe 0x005b9fd0): RenderWare Audio (Criterion Software), rwa.dll 0.95
 * Source: codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32 (src/rwa/rwaobjdef.c, function sub_23D740), a byte-identical
 * decompilation of the Xbox build of RenderWare Audio made with the same compiler (cl 13.00.9466). Used with
 * credit; split into one file per function. Changes: function names (sub_23D740 -> RwaObjDefSetInterface), the unit's data definitions are
 * extern declarations here, the CODFH_SP (single player) conditionals are resolved for the multiplayer build.
 * Byte-identical to MFL.exe 0x005b9fd0 (tools/cmpfn.py, relocations masked). */
#include "rwa.h"
#include "rwaint.h"
#include <string.h>
#include <stddef.h>
extern RwBool sub_243BC0(void *iface, rwaObjDef *def, RwInt32 index);

rwaObjDef *
RwaObjDefSetInterface(rwaObjDef *def, void *iface)
{
    if (!sub_243BC0(iface, def, 1))
    {
        return NULL;
    }

    def->interfaces[0] = iface;

    return def;
}
