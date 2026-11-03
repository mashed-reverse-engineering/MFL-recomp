/* RwaListenerCreate (MFL.exe 0x005b9500): RenderWare Audio (Criterion Software), codfh name
 * Source: codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32 (src/rwa/unit_23CC10.c, function RwaListenerCreate), a byte-identical
 * decompilation of the Xbox build of RenderWare Audio made with the same compiler (cl 13.00.9466). Used with
 * credit; split into one file per function. Changes: function names (none), the unit's data definitions are
 * extern declarations here, the CODFH_SP (single player) conditionals are resolved for the multiplayer build.
 * Byte-identical to MFL.exe 0x005b9500 (tools/cmpfn.py, relocations masked). */
#include "rwa.h"
#include "rwaint.h"
extern RwUInt8 unk_506858[];
extern RwInt32 sub_23F4C0(void *module, RwInt32 a, RwInt32 b, RwInt32 c, RwInt32 d, RwInt32 e);
extern RwUInt8 unk_506858[92];

// End of its data definitions.
RwInt32
RwaListenerCreate(RwInt32 a, RwInt32 b, RwInt32 c)
{
    return sub_23F4C0(unk_506858, 0, a, 0, b, c);
}
