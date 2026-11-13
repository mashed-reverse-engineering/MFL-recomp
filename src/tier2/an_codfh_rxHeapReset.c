/* rxHeapReset (MFL.exe 004f1830). Adapted from codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32, src/rw/src/pipe/p2/p2heap.c @e578672,
 * not byte-identical, changes: cut down to this function and the private declarations it uses, release build macros expanded (RWFUNCTION /
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

extern RwBool HeapSuperBlockReset(rxHeapSuperBlockDescriptor * superBlock, rxHeapSuperBlockDescriptor * attach2SuperBlock, RxHeap * heap);
/* TIER2: unvalidated authority=diffemu */
RwBool rxHeapReset(RxHeap * heap)
{
    rxHeapSuperBlockDescriptor *prev =
        (rxHeapSuperBlockDescriptor *) NULL;
    rxHeapSuperBlockDescriptor *iter;

    heap->entriesUsed = 0;

    iter = heap->head->next;

    while (iter != NULL)
    {
        if (!HeapSuperBlockReset(iter, prev, heap))
        {
            return (FALSE);
        }

        if (prev == NULL)
        {
            heap->headBlock = (rxHeapBlockHeader *) iter->start;
        }

        prev = iter;
        iter = iter->next;
    }

    iter = heap->head;

    if (!HeapSuperBlockReset(iter, prev, heap))
    {
        return (FALSE);
    }

    if (prev == NULL)
    {
        heap->headBlock = (rxHeapBlockHeader *) iter->start;
    }

    heap->dirty = FALSE;

    return (TRUE);
}
