/* compile: /G5 */
/* FuzzyExtentScore (MFL.exe 0x0054ecb0): RenderWare Graphics 3.7 (Criterion Software), codfh name
 * Source: codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32 (src/rw/plugin/collis/ctbuild.c, function FuzzyExtentScore), a byte-identical decompilation of the RenderWare
 * build of an Xbox game; the Win32 branches it kept are RenderWare 3.7.2 source. Used with credit; split into one file per
 * function, the declarations it needs copied from the codfh headers named in the comments (PC build: no _XBOX, no RWDEBUG).
 * Changes: no renames; data definitions of the unit are extern declarations here.
 * Byte-identical to MFL.exe 0x0054ecb0 (tools/cmpfn.py /O2 /G5, relocations masked). */
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
typedef float RwReal;
#define _RWSTRING(x) x
#define RWSTRING(x) _RWSTRING(x)
#define RWRETURN(value) return(value)
#define RWFUNCTION(name)

/******************************************************************************
 *
 *  Weight against splitting of sectors in smallest dimension. It is more
 *  efficient to have cube shaped sectors than slabs when testing lines.
 */
RwReal
FuzzyExtentScore(RwReal extent, RwReal maxExtent)
{
    RwReal              value;

    RWFUNCTION(RWSTRING("FuzzyExtentScore"));

    value = extent / maxExtent;
    if (value > ((RwReal) 0.5))
    {
        value = ((RwReal) 1);
    }
    else
    {
        /* bias against cutting small extent */
        value = ((RwReal) 0.5) + value;
    }

    value = (RwReal) 1 / value;
    RWRETURN(value);
}
