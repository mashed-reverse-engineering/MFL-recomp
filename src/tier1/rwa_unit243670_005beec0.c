/* rwa_unit243670_005beec0 and its static helpers: RenderWare Audio (Criterion Software)
 * Source: codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32 (src/rwa/unit_243670.c, functions sub_243880 (0x005bee60, static), sub_2438E0 (0x005beec0), sub_243950 (0x005bef40)), a byte-identical
 * decompilation of the Xbox build of RenderWare Audio made with the same compiler (cl 13.00.9466). Used with
 * credit; these functions stay in one file because cl 13.00 compiles a static function and its callers in the same
 * file with a register calling convention. Changes: function names (sub_243670 -> rwa_unit243670_005bec50; sub_2437A0 -> rwa_unit243670_005bed80; sub_243880 -> rwa_unit243670_005bee60; sub_2438E0 -> rwa_unit243670_005beec0; sub_243950 -> rwa_unit243670_005bef40), the unit's data definitions are
 * extern declarations here, the CODFH_SP (single player) conditionals are resolved for the multiplayer build.
 * Byte-identical to MFL.exe (tools/cmpfn.py, relocations masked): rwa_unit243670_005bee60 0x005bee60, rwa_unit243670_005beec0 0x005beec0, rwa_unit243670_005bef40 0x005bef40. Static functions keep their linkage: the
 * overlay replaces only the external ones, the static copies serve the callers in this file. */
#include "rwa.h"
#include "rwaint.h"
extern rwaLink dword_507030;
extern RwaFreeList *dword_507060;
extern void *sub_242170(RwUInt32 size, RwUInt32 hint);
extern void sub_2421B0(void *mem);
extern void *memset(void *dst, RwInt32 value, RwUInt32 size);
extern void *rwa_unit243670_005bec50(RwaFreeList *freeList, RwUInt32 hint);
extern void rwa_unit243670_005bed80(RwaFreeList *freeList, void *entry);
extern void * rwa_unit243670_005bec50(RwaFreeList *freeList, RwUInt32 hint);

/* MFL.exe 0x005bee60 */
static void
rwa_unit243670_005bee60(RwaFreeList *freeList)
{
    rwaLink    *head = &freeList->blocks;
    rwaLink    *block;

    while ((block = head->next) != head)
    {
        block->prev->next = block->next;
        block->next->prev = block->prev;
        sub_2421B0(block);
    }

    if (!(freeList->flags & 1))
    {
        if (dword_507060 != freeList && dword_507060)
        {
            rwa_unit243670_005bed80(dword_507060, freeList);
        }
        else
        {
            sub_2421B0(freeList);
        }
    }
}

/* MFL.exe 0x005beec0 */
void
rwa_unit243670_005beec0(RwaFreeList *freeList)
{
    freeList->link.prev->next = freeList->link.next;
    freeList->link.next->prev = freeList->link.prev;
    rwa_unit243670_005bee60(freeList);
}

/* MFL.exe 0x005bef40 */
RwaFreeList *
rwa_unit243670_005bef40(RwUInt32 entrySize, RwUInt32 entriesPerBlock, RwUInt32 alignment, RwUInt32 noBlocks,
           RwaFreeList *freeList, RwUInt32 hint)
{
    RwUInt32    bitmapSize;
    rwaLink    *head;

    if (!alignment)
    {
        alignment = 1;
    }

    if (!freeList)
    {
        if (dword_507060)
        {
            freeList = (RwaFreeList *)rwa_unit243670_005bec50(dword_507060, rwaMEMHINTDUR_EVENT | rwaID_800);
        }
        else
        {
            freeList = (RwaFreeList *)sub_242170(sizeof(RwaFreeList), hint);
        }

        if (!freeList)
        {
            return NULL;
        }

        freeList->flags = 2;
    }
    else
    {
        freeList->flags = 3;
    }

    entrySize = (entrySize + alignment - 1) & ~(alignment - 1);
    bitmapSize = (entriesPerBlock + 7) >> 3;
    head = &freeList->blocks;
    freeList->entrySize = entrySize;
    freeList->entriesPerBlock = entriesPerBlock;
    freeList->alignment = alignment;
    freeList->bitmapSize = bitmapSize;
    freeList->blocks.next = head;
    freeList->blocks.prev = head;

    if (noBlocks > 0)
    {
        RwUInt32    blockSize = entrySize * entriesPerBlock + bitmapSize + alignment +
                                sizeof(rwaLink);

        do
        {
            rwaLink    *block = (rwaLink *)sub_242170(blockSize, hint);

            if (!block)
            {
                rwa_unit243670_005bee60(freeList);

                return NULL;
            }

            block->prev = NULL;
            block->next = NULL;
            block->next = head->next;
            block->prev = head;
            head->next->prev = block;
            head->next = block;
            memset(block + 1, 0, bitmapSize);
        }
        while (--noBlocks);
    }

    freeList->link.next = dword_507030.next;
    freeList->link.prev = &dword_507030;
    dword_507030.next->prev = &freeList->link;
    dword_507030.next = &freeList->link;

    return freeList;
}
