/* rwa_unit243670_005bed80 (MFL.exe 0x005bed80): RenderWare Audio (Criterion Software), neutral (codfh sub_2437A0)
 * Source: codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32 (src/rwa/unit_243670.c, function sub_2437A0), a byte-identical
 * decompilation of the Xbox build of RenderWare Audio made with the same compiler (cl 13.00.9466). Used with
 * credit; split into one file per function. Changes: function names (sub_2437A0 -> rwa_unit243670_005bed80), the unit's data definitions are
 * extern declarations here, the CODFH_SP (single player) conditionals are resolved for the multiplayer build.
 * Byte-identical to MFL.exe 0x005bed80 (tools/cmpfn.py, relocations masked). */
#include "rwa.h"
#include "rwaint.h"
extern void sub_2421B0(void *mem);
extern void rwa_unit243670_005bed80(RwaFreeList *freeList, void *entry);

void
rwa_unit243670_005bed80(RwaFreeList *freeList, void *entry)
{
    RwUInt32            bitmapSize = freeList->bitmapSize;
    rwaLink            *head = &freeList->blocks;
    rwaFreeListBlock   *block;

    for (block = (rwaFreeListBlock *)head->next; &block->link != head;
         block = (rwaFreeListBlock *)block->link.next)
    {
        RwUInt8    *data = &block->bitmap[bitmapSize];

        if ((RwUInt8 *)entry >= data &&
            (RwUInt8 *)entry <= data + freeList->entriesPerBlock * freeList->entrySize)
        {
            RwUInt32    index = ((RwUInt8 *)entry - data) / freeList->entrySize;
            RwUInt32    byte = index >> 3;
            rwaLink    *first;
            RwUInt8   *heap;

            heap = block->bitmap;
            heap[byte] &= ~(0x80 >> (index - byte * 8));

            block->link.prev->next = block->link.next;
            block->link.next->prev = block->link.prev;

            first = head->next;
            if (first != head && (freeList->flags & 2))
            {
                RwUInt32    used = 0;
                RwUInt32    i;

                for (i = 0; i < bitmapSize; i++)
                {
                    used += heap[i];
                }

                if (!used)
                {
                    sub_2421B0(block);

                    return;
                }

                block->link.prev = head;
                block->link.next = first;
                head->next->prev = &block->link;
                head->next = &block->link;

                return;
            }

            block->link.next = first;
            block->link.prev = head;
            head->next->prev = &block->link;
            head->next = &block->link;

            return;
        }
    }
}
