/* sub_2498D0 (MFL.exe 0x005c4ac0): RenderWare Audio (Criterion Software), 
 * Source: codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32 (src/rwa/unit_249490.c, function sub_2498D0), a byte-identical
 * decompilation of the Xbox build of RenderWare Audio made with the same compiler (cl 13.00.9466; MFL.exe was built with cl 13.10). Used with
 * credit; split into one file per function. Changes: function names (none), the unit's data definitions are
 * extern declarations here, the CODFH_SP (single player) conditionals are resolved for the multiplayer build.
 * Byte-identical to MFL.exe 0x005c4ac0 (tools/cmpfn.py, relocations masked). */
/* compile: cl1310 */
#include "rwa.h"
#include "rwaint.h"
#include <math.h>
extern RwUInt32 sub_2497A0(const RwaWaveFormat *from, const void *src, const RwaWaveFormat *to, RwUInt32 srcSize, RwUInt32 *used);

RwUInt32
sub_2498D0(const RwaWaveFormat *from, const void *src, RwUInt32 srcSize, const RwaWaveFormat *to)
{
    RwUInt32    used;

    sub_2497A0(from, src, to, srcSize, &used);
    return used;
}
