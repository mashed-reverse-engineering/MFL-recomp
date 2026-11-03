/* RwaObjDefAssignID (MFL.exe 0x005ba000): RenderWare Audio (Criterion Software), codfh name
 * Source: codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32 (src/rwa/rwaobjdef.c, function RwaObjDefAssignID), a byte-identical
 * decompilation of the Xbox build of RenderWare Audio made with the same compiler (cl 13.00.9466). Used with
 * credit; split into one file per function. Changes: function names (none), the unit's data definitions are
 * extern declarations here, the CODFH_SP (single player) conditionals are resolved for the multiplayer build.
 * Byte-identical to MFL.exe 0x005ba000 (tools/cmpfn.py, relocations masked). */
#include "rwa.h"
#include "rwaint.h"
#include <string.h>
#include <stddef.h>
extern void sub_240BF0(rwaObjDef *def, const void *textID);
extern void sub_240C10(rwaObjDef *def, const void *uuid);

rwaObjDef *
RwaObjDefAssignID(rwaObjDef *def, const void *uuid, const void *textID)
{
    sub_240BF0(def, textID);
    sub_240C10(def, uuid);

    return def;
}
