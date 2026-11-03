/* rwa_unit243670_005bec50 (MFL.exe 0x005bec50): RenderWare Audio (Criterion Software), neutral (codfh sub_243670)
 * Source: codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32 (src/rwa/unit_243670.c, function sub_243670), a byte-identical
 * decompilation of the Xbox build of RenderWare Audio made with the same compiler (cl 13.00.9466). Used with
 * credit; split into one file per function. Changes: function names (sub_243670 -> rwa_unit243670_005bec50), the unit's data definitions are
 * extern declarations here, the CODFH_SP (single player) conditionals are resolved for the multiplayer build.
 * Byte-identical to MFL.exe 0x005bec50 (tools/cmpfn.py, relocations masked). */
#include "rwa.h"
#include "rwaint.h"
extern void *sub_242170(RwUInt32 size, RwUInt32 hint);
extern void *memset(void *dst, RwInt32 value, RwUInt32 size);
extern void *rwa_unit243670_005bec50(RwaFreeList *freeList, RwUInt32 hint);
static __forceinline RwUInt32
rwaFreeListBlockData(rwaFreeListBlock *block, RwUInt32 size, RwUInt32 alignment)
{
    return ((RwUInt32)block + sizeof(rwaLink) + size + alignment - 1) &
           ~(alignment - 1);
}

void *
rwa_unit243670_005bec50(RwaFreeList *freeList, RwUInt32 hint)
{
    RwUInt32            bitmapSize = freeList->bitmapSize;
    rwaLink            *head = &freeList->blocks;
    rwaFreeListBlock   *block = (rwaFreeListBlock *)head->next;
    void              *entry = NULL;
    RwUInt32           checkEntries;

    while (&block->link != head)
    {
        RwUInt32    i;
        RwUInt8   *heap = block->bitmap;

        if (entry)
        {
            return (void *)entry;
        }

        checkEntries = freeList->entriesPerBlock;
        for (i = 0; i < bitmapSize; i++)
        {
            RwUInt8     bits = heap[i];
            RwUInt32    bit;

            if (bits != 0xFF)
            {
                for (bit = 0; bit < 8; bit++, checkEntries--)
                {
                    RwUInt8     mask;

                    if (checkEntries <= 0)
                    {
                        break;
                    }

                    mask = (RwUInt8)(0x80 >> bit);
                    if (!(bits & mask))
                    {
                        heap[i] = (RwUInt8)(heap[i] | mask);
                        entry = (void *)(rwaFreeListBlockData(block, bitmapSize, freeList->alignment) +
                                (bit + i * 8) * freeList->entrySize);
                        break;
                    }
                }
            }
            else
            {
                checkEntries -= 8;
            }
            if (entry)
            {
                break;
            }
        }

        block = (rwaFreeListBlock *)block->link.next;
    }

    if (entry)
    {
        return (void *)entry;
    }

    block = (rwaFreeListBlock *)sub_242170(freeList->entriesPerBlock * freeList->entrySize +
                                           freeList->alignment + bitmapSize +
                                           sizeof(rwaLink), hint);
    if (!block)
    {
        return NULL;
    }

    memset(block->bitmap, 0, bitmapSize);
    block->link.next = head->next;
    block->link.prev = head;
    head->next->prev = &block->link;
    head->next = &block->link;
    block->bitmap[0] = 0x80;

    return (void *)rwaFreeListBlockData(block, bitmapSize, freeList->alignment);
}
