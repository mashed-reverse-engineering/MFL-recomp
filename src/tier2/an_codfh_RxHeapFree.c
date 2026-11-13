/* RxHeapFree (MFL.exe 004f13d0). Adapted from codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32, src/rw/src/pipe/p2/p2heap.c @e578672,
 * not byte-identical, changes: cut down to this function and the private declarations it uses, the static helper
 * HeapFreeBlocksDeleteEntry inlined by hand (MFL.exe has it inlined), the empty debug macro CHKHEAP dropped, release build macros expanded (RWFUNCTION /
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

extern rxHeapFreeBlock * HeapFreeBlocksNewEntry(RxHeap *heap);
/* TIER2: unvalidated authority=diffemu */
void RxHeapFree(RxHeap * heap, void *block)
{
    rxHeapBlockHeader  *blockHdr;
    RwBool              mergeBack;
    RwBool              mergeFwd;

    blockHdr = ((rxHeapBlockHeader *) block) - 1;
    mergeBack =
        (blockHdr->prev != NULL) && (blockHdr->prev->freeEntry != NULL);
    mergeFwd =
        (blockHdr->next != NULL) && (blockHdr->next->freeEntry != NULL);
    if (mergeBack)
    {
        if (mergeFwd)
        {
            {   /* HeapFreeBlocksDeleteEntry(heap, blockHdr->next->freeEntry), inlined as in MFL.exe */
                rxHeapFreeBlock *freeEntry = blockHdr->next->freeEntry;

                if (&heap->freeBlocks[heap->entriesUsed - 1] != freeEntry)
                {
                    *freeEntry = heap->freeBlocks[heap->entriesUsed - 1];

                    freeEntry->ptr->freeEntry = freeEntry;
                }

                --heap->entriesUsed;
            }

            blockHdr->prev->size +=
                blockHdr->size + blockHdr->next->size +
                sizeof(rxHeapBlockHeader) * 2;
            blockHdr->prev->freeEntry->size = blockHdr->prev->size;
            blockHdr->prev->next = blockHdr->next->next;
            if (blockHdr->next->next != NULL)
            {
                blockHdr->next->next->prev = blockHdr->prev;
            }        }
        else
        {
            
            blockHdr->prev->size +=
                blockHdr->size + sizeof(rxHeapBlockHeader);
            blockHdr->prev->freeEntry->size = blockHdr->prev->size;
            blockHdr->prev->next = blockHdr->next;
            if (blockHdr->next != NULL)
            {
                blockHdr->next->prev = blockHdr->prev;
            }        }
    }
    else
    {
        if (mergeFwd)
        {
            
            blockHdr->size +=
                blockHdr->next->size + sizeof(rxHeapBlockHeader);
            blockHdr->freeEntry = blockHdr->next->freeEntry;
            blockHdr->next->freeEntry->ptr = blockHdr;
            blockHdr->next->freeEntry->size = blockHdr->size;
            blockHdr->next = blockHdr->next->next;
            if (blockHdr->next != NULL)
            {
                blockHdr->next->prev = blockHdr;
            }        }
        else
        {
            
            rxHeapFreeBlock *freeEntry = HeapFreeBlocksNewEntry(heap);

            if (freeEntry == NULL)
            {

                return;
            }

            freeEntry->ptr = blockHdr;
            freeEntry->size = blockHdr->size;

            blockHdr->freeEntry = freeEntry;        }
    }

    return;
}
