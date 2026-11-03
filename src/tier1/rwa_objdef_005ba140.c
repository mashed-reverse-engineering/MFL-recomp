/* rwa_objdef_005ba140 (MFL.exe 0x005ba140): RenderWare Audio (Criterion Software), neutral (codfh sub_23D8B0)
 * Source: codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32 (src/rwa/rwaobjdef.c, function sub_23D8B0), a byte-identical
 * decompilation of the Xbox build of RenderWare Audio made with the same compiler (cl 13.00.9466). Used with
 * credit; split into one file per function. Changes: function names (sub_23D8B0 -> rwa_objdef_005ba140; sub_2437A0 -> rwa_unit243670_005bed80), the unit's data definitions are
 * extern declarations here, the CODFH_SP (single player) conditionals are resolved for the multiplayer build.
 * Byte-identical to MFL.exe 0x005ba140 (tools/cmpfn.py, relocations masked). */
#include "rwa.h"
#include "rwaint.h"
#include <string.h>
#include <stddef.h>
extern RwUInt32 sub_23F300(rwaObjDef *def, RwUInt32 flags, RwUInt32 *stateOffset);
extern void rwa_unit243670_005bed80(void *freeList, void *entry);
extern void sub_2444A0(void *pool, void *entry);
extern void sub_2421B0(void *entry);

void
rwa_objdef_005ba140(rwaInstance *input, void *callback, void *data)
{
    void (*release)(void *, void *) = (void (*)(void *, void *))callback;
    rwaInstance *obj = input;
    RwUInt32 offset;  /* unused state offset; compiler reuses the input argument home */
    rwaObjDef *def = obj->def;
    void *memory = obj;

    if (obj->flags & 8)
    {
        memory = ((void **)obj)[-1];
    }
    else if (obj->flags & 0x10)
    {
        RwUInt8 *storage = (RwUInt8 *)obj;
        RwUInt32 size = sub_23F300(def, obj->flags & 7, &offset);
        /* Allocation pointer follows the variable-sized instance payload. */
        storage += size;
        memory = *(void **)storage;
    }

    if (def->instances && (!release || !(obj->flags & 0x20)))
    {
        if (def->flags0C & 0x40)
        {
            rwa_unit243670_005bed80(def->instances, memory);
        }
        else if (def->flags0C & 0x80)
        {
            sub_2444A0(def->instances, memory);
        }
        else
        {
            sub_2421B0(memory);
        }
    }
    else if (release)
    {
        release(memory, data);
    }
}
