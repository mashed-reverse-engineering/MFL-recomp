/* rwa_objdef_005ba200 (MFL.exe 0x005ba200): RenderWare Audio (Criterion Software), neutral (codfh sub_23D970)
 * Source: codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32 (src/rwa/rwaobjdef.c, function sub_23D970), a byte-identical
 * decompilation of the Xbox build of RenderWare Audio made with the same compiler (cl 13.00.9466). Used with
 * credit; split into one file per function. Changes: function names (sub_23D970 -> rwa_objdef_005ba200), the unit's data definitions are
 * extern declarations here, the CODFH_SP (single player) conditionals are resolved for the multiplayer build.
 * Byte-identical to MFL.exe 0x005ba200 (tools/cmpfn.py, relocations masked). */
#include "rwa.h"
#include "rwaint.h"
#include <string.h>
#include <stddef.h>
extern rwaObjDef *dword_516C60;
typedef RwInt32 (*rwaObjDefSearchFunc)(rwaObjDef *parent, rwaObjDef *child, void *data);
rwaObjDef *rwa_objdef_005ba200(rwaObjDef *def, rwaObjDef **parent, rwaObjDefSearchFunc callback,
                       void *data, RwInt32 self);

/* The two qualified argument-home reads preserve the parent reloads
 * after the callback and on its successful child path. Original
 * declaration qualifiers are unknown. */
rwaObjDef *
rwa_objdef_005ba200(rwaObjDef *def, rwaObjDef **parent, rwaObjDefSearchFunc callback, void *data,
           RwInt32 self)
{
    rwaLink    *cur;
    rwaObjDef *root = def;
    rwaLink *head;

    if (!root)
    {
        root = dword_516C60;
    }
    def = root;

    if (self == 1 && callback(root, root, data) == 1)
    {
        if (parent)
        {
            *parent = root;
        }

        return root;
    }

    head = &root->children;
    for (cur = head->next; cur != head; cur = cur->next)
    {
        rwaObjDef  *child = (rwaObjDef *)((RwUInt8 *)cur - offsetof(rwaObjDef, sibling));
        rwaObjDef  *found;

        if (callback(*(rwaObjDef *volatile *)&def, child, data) == 1)
        {
            if (parent)
            {
                *parent = *(rwaObjDef *volatile *)&def;
            }

            return child;
        }

        found = rwa_objdef_005ba200(child, parent, callback, data, 0);
        if (found)
        {
            return found;
        }
    }

    if (parent)
    {
        *parent = NULL;
    }

    return NULL;
}
