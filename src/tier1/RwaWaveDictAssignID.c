/* RwaWaveDictAssignID (MFL.exe 0x005bd930): RenderWare Audio (Criterion Software), codfh name
 * Source: codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32 (src/rwa/unit_243B40.c, function RwaWaveDictAssignID), a byte-identical
 * decompilation of the Xbox build of RenderWare Audio made with the same compiler (cl 13.00.9466). Used with
 * credit; split into one file per function. Changes: function names (none), the unit's data definitions are
 * extern declarations here, the CODFH_SP (single player) conditionals are resolved for the multiplayer build.
 * Byte-identical to MFL.exe 0x005bd930 (tools/cmpfn.py, relocations masked). */
#include "rwa.h"
#include "rwaint.h"

RwaObjInterfaceDef *
RwaWaveDictAssignID(RwaObjInterfaceDef *def, const RwaUUID *uuid, const RwChar *name)
{
    sub_240C10(&def->uniqueID, uuid);
    sub_240BF0(&def->uniqueID, name);

    return def;
}
