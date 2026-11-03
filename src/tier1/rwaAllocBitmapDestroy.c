/* rwaAllocBitmapDestroy (MFL.exe 0x005da720): RenderWare Audio (Criterion Software), codfh name
 * Source: codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32 (src/rwa/unit_2FD5D0.c, function rwaAllocBitmapDestroy), a byte-identical
 * decompilation of the Xbox build of RenderWare Audio made with the same compiler (cl 13.00.9466). Used with
 * credit; split into one file per function. Changes: function names (sub_2437A0 -> rwa_unit243670_005bed80), the unit's data definitions are
 * extern declarations here, the CODFH_SP (single player) conditionals are resolved for the multiplayer build.
 * Byte-identical to MFL.exe 0x005da720 (tools/cmpfn.py, relocations masked). */
#include "rwa.h"
#include "rwaint.h"
extern RwaFreeList unk_5125EC;
extern void sub_2421B0(void *mem);
extern void rwa_unit243670_005bed80(RwaFreeList *freeList, void *entry);
extern RwaFreeList unk_5125EC;      /* the bitmaps' free list */

void
rwaAllocBitmapDestroy(RwaAllocBitmap *bitmap)
{
    if (bitmap->flags & 2)
    {
        sub_2421B0(bitmap->bits);
    }
    if (bitmap->flags & 1)
    {
        rwa_unit243670_005bed80(&unk_5125EC, bitmap);
    }
}
