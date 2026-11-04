/* sub_242050 (MFL.exe 0x005bf460): RenderWare Audio (Criterion Software), 
 * Source: codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32 (src/rwa/unit_241F90.c, function sub_242050), a byte-identical
 * decompilation of the Xbox build of RenderWare Audio made with the same compiler (cl 13.00.9466; MFL.exe was built with cl 13.10). Used with
 * credit; split into one file per function. Changes: function names (none), the unit's data definitions are
 * extern declarations here, the CODFH_SP (single player) conditionals are resolved for the multiplayer build.
 * Byte-identical to MFL.exe 0x005bf460 (tools/cmpfn.py, relocations masked). */
/* compile: cl1310 */
#include "rwa.h"
#include <math.h>
#include "rwaint.h"
extern RwInt16 word_42F1A0[768];
#define rwaSqrtInPlace(_root)                                               \
    if (*(_root) != 0.0f)                                                   \
    {                                                                       \
        RwUInt32   *_bits = (RwUInt32 *)(_root);                            \
        RwInt16     _exp = (RwInt16)((*_bits >> 23) - 127);                 \
                                                                            \
        *_bits &= 0x7FFFFF;                                                 \
        if (_exp & 1)                                                       \
        {                                                                   \
            *_bits |= 0x800000;                                             \
        }                                                                   \
                                                                            \
        _exp >>= 1;                                                         \
        *_bits = ((RwUInt32)(RwInt32)word_42F1A0[*_bits >> 16] << 16) |     \
                 ((RwUInt32)(_exp + 127) << 23);                            \
    }
#define rwaV3dDotProductMacro(a, b)                              \
    ((((( (((a)->x) * ((b)->x))) +                              \
        ( (((a)->y) * ((b)->y))))) +                            \
        ( (((a)->z) * ((b)->z)))))

void
sub_242050(RwReal *length, const RwaV3d *vector)
{
    *length = rwaV3dDotProductMacro(vector, vector);
    rwaSqrtInPlace(length);
}
