/* HeapSuperBlockReset (MFL.exe 004f1710). Adapted from codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32, src/rw/src/pipe/p2/p2heap.c @e578672,
 * not byte-identical, changes: cut down to this function and the private declarations it uses, release build macros expanded (RWFUNCTION /
 * RWASSERT dropped, RWRETURN -> return), conditional compilation resolved for the release D3D9 build, the SDK includes replaced by rw37_slim.h,
 * data and functions MFL.exe names declared extern at their addresses. Generated, then hand-checked; compared with MFL.exe by differential
 * emulation (tier 2). */
#include "rw37_slim.h"

extern rxHeapFreeBlock * HeapFreeBlocksNewEntry(RxHeap *heap);
/* TIER2: unvalidated authority=diffemu */
RwBool HeapSuperBlockReset(rxHeapSuperBlockDescriptor * superBlock, rxHeapSuperBlockDescriptor * attach2SuperBlock, RxHeap * heap)
{
    rxHeapFreeBlock *freeEntry;

    freeEntry = HeapFreeBlocksNewEntry(heap);

    if (NULL != freeEntry)
    {
        rxHeapBlockHeader  *blockHdrBeg, *blockHdrPrincipal,
            *blockHdrEnd;

        blockHdrBeg = (rxHeapBlockHeader *) superBlock->start;
        blockHdrPrincipal = &blockHdrBeg[1];
        blockHdrEnd = &((rxHeapBlockHeader *)
                        (((RwUInt8 *) superBlock->start) +
                         superBlock->size))[-1];

        blockHdrBeg->prev = (rxHeapBlockHeader *) NULL;
        blockHdrBeg->next = (rxHeapBlockHeader *) NULL;
        blockHdrBeg->size = 0;
        blockHdrBeg->freeEntry = (rxHeapFreeBlock *) NULL;

        *blockHdrEnd = *blockHdrBeg;

        blockHdrBeg->next = blockHdrPrincipal;
        blockHdrPrincipal->prev = blockHdrBeg;

        blockHdrPrincipal->next = blockHdrEnd;
        blockHdrEnd->prev = blockHdrPrincipal;

        blockHdrPrincipal->size =
            ((RwUInt8 *) blockHdrEnd) -
            ((RwUInt8 *) & blockHdrPrincipal[1]);
        blockHdrPrincipal->freeEntry = freeEntry;

        freeEntry->ptr = blockHdrPrincipal;
        freeEntry->size = blockHdrPrincipal->size;

        if (attach2SuperBlock != NULL)
        {
            rxHeapBlockHeader  *blockHdrAttach2 =
                &((rxHeapBlockHeader *)
                  (((RwUInt8 *) attach2SuperBlock->start) +
                   attach2SuperBlock->size))[-1];

            blockHdrAttach2->next = blockHdrBeg;
            blockHdrBeg->prev = blockHdrAttach2;
        }

        return (TRUE);
    }

    return (FALSE);
}
