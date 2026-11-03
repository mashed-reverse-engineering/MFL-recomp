/* rwa_uuid_005be770 (MFL.exe 0x005be770): RenderWare Audio (Criterion Software), neutral (codfh sub_240B90)
 * Source: codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32 (src/rwa/rwauuid.c, function sub_240B90), a byte-identical
 * decompilation of the Xbox build of RenderWare Audio made with the same compiler (cl 13.00.9466). Used with
 * credit; split into one file per function. Changes: function names (sub_240B90 -> rwa_uuid_005be770), the unit's data definitions are
 * extern declarations here, the CODFH_SP (single player) conditionals are resolved for the multiplayer build.
 * Byte-identical to MFL.exe 0x005be770 (tools/cmpfn.py, relocations masked). */
#include "rwa.h"
#include "rwaint.h"
#define rwaFoldCase1(_c)                                                    \
    ((((_c) >= 'a' || (_c) >= 'A') && ((_c) <= 'z' || (_c) <= 'Z')) ?       \
     (RwChar)((_c) & 0xDF) : (_c))
#define rwaFoldCase2(_c)                                                    \
    ((((_c) >= 'a' || (_c) <= 'A') && ((_c) <= 'z' || (_c) <= 'Z')) ?       \
     (RwChar)((_c) & 0xDF) : (_c))

RwInt32
rwa_uuid_005be770(const RwChar *string1, const RwChar *string2, RwUInt32 count)
{
    do
    {
        RwChar  c1 = rwaFoldCase1(*string1);
        RwChar  c2 = rwaFoldCase2(*string2);

        if (c1 != c2)
        {
            return c1 - c2;
        }

        string1++;
        string2++;
    }
    while (--count);

    return 0;
}
