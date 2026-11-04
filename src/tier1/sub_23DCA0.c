/* sub_23DCA0 (MFL.exe 0x005ba5c0): RenderWare Audio (Criterion Software), 
 * Source: codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32 (src/rwa/rwaobjdef.c, function sub_23DCA0), a byte-identical
 * decompilation of the Xbox build of RenderWare Audio made with the same compiler (cl 13.00.9466; MFL.exe was built with cl 13.10). Used with
 * credit; split into one file per function. Changes: function names (sub_23D970 -> rwa_objdef_005ba200; sub_23DA80 -> rwa_objdef_005ba310), the unit's data definitions are
 * extern declarations here, the CODFH_SP (single player) conditionals are resolved for the multiplayer build.
 * Byte-identical to MFL.exe 0x005ba5c0 (tools/cmpfn.py, relocations masked). */
/* compile: cl1310 */
#include "rwa.h"
#include "rwaint.h"
#include <string.h>
#include <stddef.h>
typedef RwInt32 (*rwaObjDefSearchFunc)(rwaObjDef *parent, rwaObjDef *child, void *data);
rwaObjDef *rwa_objdef_005ba200(rwaObjDef *def, rwaObjDef **parent, rwaObjDefSearchFunc callback,
                       void *data, RwInt32 self);
extern rwaObjDef * rwa_objdef_005ba200(rwaObjDef *def, rwaObjDef **parent, rwaObjDefSearchFunc callback, void *data, RwInt32 self);
extern RwInt32 rwa_objdef_005ba310(rwaObjDef *parent, rwaObjDef *child, const RwaUUID *uuid);

rwaObjDef *
sub_23DCA0(rwaObjDef *root, const RwaUUID *uuid, rwaObjDef **parentOut)
{
    RwaUUID     id = *uuid;
    rwaObjDef  *parent;
    rwaObjDef  *found;

    found = rwa_objdef_005ba200(root, &parent, (rwaObjDefSearchFunc)rwa_objdef_005ba310, &id, 1);

    if (parentOut)
    {
        *parentOut = parent;
    }

    return found;
}
