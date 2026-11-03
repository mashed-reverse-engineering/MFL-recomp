/* RwaWaveDefAssignID (MFL.exe 0x005bc030): RenderWare Audio (Criterion Software), codfh name
 * Source: codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32 (src/rwa/unit_242910.c, function RwaWaveDefAssignID), a byte-identical
 * decompilation of the Xbox build of RenderWare Audio made with the same compiler (cl 13.00.9466). Used with
 * credit; split into one file per function. Changes: function names (none), the unit's data definitions are
 * extern declarations here, the CODFH_SP (single player) conditionals are resolved for the multiplayer build.
 * Byte-identical to MFL.exe 0x005bc030 (tools/cmpfn.py, relocations masked). */
#include "rwa.h"
#include "rwaint.h"
extern RwaUniqueID *sub_240BF0(RwaUniqueID *uniqueID, const RwChar *name);
extern RwaUniqueID *sub_240C10(RwaUniqueID *uniqueID, const RwaUUID *uuid);

RwaWaveDef *
RwaWaveDefAssignID(RwaWaveDef *waveDef, const RwaUUID *uuid, const RwChar *name)
{
    if (!sub_240C10(&waveDef->uniqueID, uuid))
    {
        return NULL;
    }

    return sub_240BF0(&waveDef->uniqueID, name) ? waveDef : NULL;
}
