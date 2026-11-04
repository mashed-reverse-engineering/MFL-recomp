/* sub_2FD5D0 (MFL.exe 0x005da620): RenderWare Audio (Criterion Software), 
 * Source: codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32 (src/rwa/unit_2FD5D0.c, function sub_2FD5D0), a byte-identical
 * decompilation of the Xbox build of RenderWare Audio made with the same compiler (cl 13.00.9466; MFL.exe was built with cl 13.10). Used with
 * credit; split into one file per function. Changes: function names (sub_243670 -> rwa_unit243670_005bec50; sub_2437A0 -> rwa_unit243670_005bed80), the unit's data definitions are
 * extern declarations here, the CODFH_SP (single player) conditionals are resolved for the multiplayer build.
 * Byte-identical to MFL.exe 0x005da620 (tools/cmpfn.py, relocations masked). */
/* compile: cl1310 */
#include "rwa2.h"
#include "rwaint2.h"
extern RwaFreeList unk_5125EC;
extern void *sub_242170(RwUInt32 size, RwUInt32 hint);
extern void *memset(void *dst, RwInt32 value, RwUInt32 size);
extern void *rwa_unit243670_005bec50(RwaFreeList *freeList, RwUInt32 hint);
extern void rwa_unit243670_005bed80(RwaFreeList *freeList, void *entry);
extern RwaFreeList unk_5125EC;      /* the bitmaps' free list */
#define rwaLog2U16(_v) \
    (((_v) & 0xFF00) ? _rwMsbBit[(_v) >> 8] + 7 : _rwMsbBit[(_v)] - 1)
#define rwaLog2U(_x) \
    (((_x) & 0xFFFF0000) ? rwaLog2U16((_x) >> 16) + 16 : rwaLog2U16(_x))

RwaAllocBitmap *
sub_2FD5D0(RwUInt32 numUnits, RwaAllocBitmap *bitmap, RwUInt8 *bits)
{
    RwUInt8     flags = 0;
    RwUInt32    rounded = (numUnits + 7) & ~7;
    RwUInt8     shift;

    shift = (RwUInt8)(rwaLog2U(rounded) + 1);
    rounded >>= 3;

    if (!bitmap)
    {
        bitmap = (RwaAllocBitmap *)rwa_unit243670_005bec50(&unk_5125EC, rwaMEMHINTDUR_EVENT | rwaID_800);
        flags = 1;
    }
    if (bitmap)
    {
        if (!bits)
        {
            bits = (RwUInt8 *)sub_242170(rounded, rwaMEMHINTDUR_EVENT);
            flags |= 2;
        }
        if (bits)
        {
            bitmap->shift = shift;
            bitmap->bits = bits;
            memset(bits, 0, rounded);
            bitmap->numBytes = rounded;
            bitmap->flags = flags;
        }
        else
        {
            if (flags & 1)
            {
                rwa_unit243670_005bed80(&unk_5125EC, bitmap);
            }
            bitmap = NULL;
        }
    }
    return bitmap;
}
