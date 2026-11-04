/* compile: /G5 */
/* StrICmp (MFL.exe 0x004ea320): RenderWare Graphics 3.7 (Criterion Software), codfh name
 * Source: codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32 (src/rw/src/plcore/rwstring.c, function StrICmp), a byte-identical decompilation of the RenderWare
 * build of an Xbox game; the Win32 branches it kept are RenderWare 3.7.2 source. Used with credit; split into one file per
 * function, the declarations it needs copied from the codfh headers named in the comments (PC build: no _XBOX, no RWDEBUG).
 * Changes: no renames; data definitions of the unit are extern declarations here.
 * Byte-identical to MFL.exe 0x004ea320 (tools/cmpfn.py /O2 /G5, relocations masked). */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stddef.h>
#include <math.h>
#include <stdarg.h>
typedef char RwChar;
#define _RWSTRING(x) x
#define RWSTRING(x) _RWSTRING(x)
#define RWRETURN(value) return(value)
#define RWFUNCTION(name)       /* No op */

/* Implement some string functions for platforms which don't have them,
 * or they just plain don't work.
 */
// Its data (PLAN M5): literals, constants and tables where the image has them.
int
StrICmp(const RwChar * s1, const RwChar * s2)
{
    RWFUNCTION(RWSTRING("StrICmp"));

    if (s1 && s2)
    {
        RwChar          c1, c2;

        do
        {
            c1 = *s1;
            c2 = *s2;

            /* Convert to lower case for comparison */
            if ((c1 >= 'A') && (c1 <= 'Z'))
            {
                c1 += 'a' - 'A';
            }

            if ((c2 >= 'A') && (c2 <= 'Z'))
            {
                c2 += 'a' - 'A';
            }

            /* Compare - anything beats NUL!!! */
            if (c1 != c2)
            {
                RWRETURN(c1 - c2);
            }

            /* Next char */
            ++s1;
            ++s2;
        }
        while (c1 && c2);

        if (c1 != c2)
        {
            RWRETURN(c1 - c2);
        }
    }

    /* Same or not got two strings */
    RWRETURN(0);
}
