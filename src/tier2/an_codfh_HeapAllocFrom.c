/* HeapAllocFrom (MFL.exe 004f1790). Adapted from codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32, src/rw/src/pipe/p2/p2heap.c @e578672,
 * not byte-identical, changes: cut down to this function and the private declarations it uses, the static helper
 * HeapFreeBlocksDeleteEntry inlined by hand (MFL.exe has it inlined), the empty debug macros CHKHEAP / FLOG dropped, release build macros expanded (RWFUNCTION /
 * RWASSERT dropped, RWRETURN -> return), conditional compilation resolved for the release D3D9 build, the SDK includes replaced by rw37_slim.h,
 * data and functions MFL.exe names declared extern at their addresses. Generated, then hand-checked; compared with MFL.exe by differential
 * emulation (tier 2). */
#include "rw37_slim.h"
/* ---- SDK declarations only this row uses (generated together with rw37_slim.h: do not edit between the markers) ---- */
struct RxHeap
{
    RwUInt32 superBlockSize;
    rxHeapSuperBlockDescriptor *head;
    rxHeapBlockHeader *headBlock;
    rxHeapFreeBlock *freeBlocks;
    RwUInt32 entriesAlloced;
    RwUInt32 entriesUsed;
    RwBool dirty;

};
RW_LAYOUT_ASSERT(rw37_row_0, RW_OFFSETOF(RxHeap, freeBlocks) == 0xc && RW_OFFSETOF(RxHeap, entriesUsed) == 0x14);      /* MFL.exe 0x4f17ae / 0x4f17b1 / 0x4f17d3 (HeapAllocFrom): heap->entriesUsed [esi+0x14], heap->freeBlocks [esi+0xc]; the same at 0x4f141b / 0x4f141e (RxHeapFree) */
/* ---- end of the generated declarations ---- */

/* TIER2: unvalidated authority=diffemu */
void * HeapAllocFrom(RxHeap * heap, rxHeapFreeBlock * freeEntry, RwUInt32 size)
{
    
    RwUInt32            delta;

    delta = freeEntry->size - size;

    if (delta < 256)
    {
        rxHeapBlockHeader *blockHdr = freeEntry->ptr;

        /* HeapFreeBlocksDeleteEntry, inlined as in MFL.exe: the last entry moves into the hole */
        if (&heap->freeBlocks[heap->entriesUsed - 1] != freeEntry)
        {
            *freeEntry = heap->freeBlocks[heap->entriesUsed - 1];

            freeEntry->ptr->freeEntry = freeEntry;
        }

        --heap->entriesUsed;

        blockHdr->freeEntry = (rxHeapFreeBlock *) NULL;

        return ((void *) &blockHdr[1]);
    }
    else
    {

        rxHeapBlockHeader  *blockHdr = freeEntry->ptr;
        rxHeapBlockHeader  *newBlock =
            (rxHeapBlockHeader *) (((RwUInt8 *) & blockHdr[1]) + size);

        newBlock->size = delta - sizeof(rxHeapBlockHeader);
        newBlock->prev = blockHdr;
        newBlock->next = blockHdr->next;
        newBlock->freeEntry = freeEntry;

        blockHdr->size = size;
        blockHdr->next = newBlock;

        if (newBlock->next != NULL)
        {
            newBlock->next->prev = newBlock;
        }

        freeEntry->ptr = newBlock;
        freeEntry->size = newBlock->size;

        blockHdr->freeEntry = (rxHeapFreeBlock *) NULL;

        return ((void *) &blockHdr[1]);
    }
}
