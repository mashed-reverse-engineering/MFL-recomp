/* _rwaStreamNodeProcessComplete (MFL.exe 0x005c1d00): RenderWare Audio (Criterion Software), rwa.dll 0.963
 * Source: codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32 (src/rwa/unit_246FD0.c, function sub_2470A0), a byte-identical
 * decompilation of the Xbox build of RenderWare Audio made with the same compiler (cl 13.00.9466). Used with
 * credit; split into one file per function. Changes: function names (sub_2470A0 -> _rwaStreamNodeProcessComplete), the unit's data definitions are
 * extern declarations here, the CODFH_SP (single player) conditionals are resolved for the multiplayer build.
 * Byte-identical to MFL.exe 0x005c1d00 (tools/cmpfn.py, relocations masked). */
#include "rwa.h"
#include "rwaint.h"

/* The node's status from its mode, unlinking it from its parent when it
 * plays. */
void
_rwaStreamNodeProcessComplete(rwaDecoder *obj)
{
    switch (obj->mode)
    {
    case 1:
        if (obj->inst.subObjLink.next)
        {
            obj->inst.subObjLink.prev->next = obj->inst.subObjLink.next;
            obj->inst.subObjLink.next->prev = obj->inst.subObjLink.prev;
            obj->inst.subObjLink.prev = NULL;
            obj->inst.subObjLink.next = NULL;
        }
        obj->status = 3;
        break;
    case 2:
        obj->status = 6;
        break;
    case 3:
        obj->status = 5;
        break;
    }
}
