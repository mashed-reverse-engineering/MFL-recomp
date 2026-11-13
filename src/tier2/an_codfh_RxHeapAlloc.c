/* RxHeapAlloc (MFL.exe 004f15c0). Adapted from codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32, src/rw/src/pipe/p2/p2heap.c @e578672
 *, not byte-identical, changes: cut down to this function and the private declarations it uses,
 * HeapGrow / HeapSuperBlockCreate / HeapSuperBlockDestroy inlined by hand (MFL.exe has them inlined), the alignment cast through mfl_uptr,
 * release build macros expanded (RWFUNCTION / RWASSERT dropped, RWRETURN -> return), conditional compilation resolved for the release D3D9
 * build, the SDK includes replaced by rw37_slim.h, data and functions MFL.exe names declared extern at their addresses. Generated, then
 * hand-checked; compared with MFL.exe by differential emulation (tier 2). */
#include "rw37_slim.h"
/* ---- SDK declarations only this row uses (generated together with rw37_slim.h: do not edit between the markers) ---- */
#define _RWSTRING(x) x
#define RWSTRING(x) _RWSTRING(x)
enum RwCriterionCoreID
{

    rwID_NACOREID = MAKECHUNKID(rwVENDORID_CRITERIONINT, 0x00),

    rwID_VECTORMODULE = MAKECHUNKID(rwVENDORID_CRITERIONINT, 0x01),

    rwID_MATRIXMODULE = MAKECHUNKID(rwVENDORID_CRITERIONINT, 0x02),

    rwID_FRAMEMODULE = MAKECHUNKID(rwVENDORID_CRITERIONINT, 0x03),

    rwID_STREAMMODULE = MAKECHUNKID(rwVENDORID_CRITERIONINT, 0x04),

    rwID_CAMERAMODULE = MAKECHUNKID(rwVENDORID_CRITERIONINT, 0x05),

    rwID_IMAGEMODULE = MAKECHUNKID(rwVENDORID_CRITERIONINT, 0x06),

    rwID_RASTERMODULE = MAKECHUNKID(rwVENDORID_CRITERIONINT, 0x07),

    rwID_TEXTUREMODULE = MAKECHUNKID(rwVENDORID_CRITERIONINT, 0x08),

    rwID_PIPEMODULE = MAKECHUNKID(rwVENDORID_CRITERIONINT, 0x09),

    rwID_IMMEDIATEMODULE = MAKECHUNKID(rwVENDORID_CRITERIONINT, 0x0A),

    rwID_RESOURCESMODULE = MAKECHUNKID(rwVENDORID_CRITERIONINT, 0x0B),

    rwID_DEVICEMODULE = MAKECHUNKID(rwVENDORID_CRITERIONINT, 0x0C),

    rwID_COLORMODULE = MAKECHUNKID(rwVENDORID_CRITERIONINT, 0x0D),

    rwID_POLYPIPEMODULE = MAKECHUNKID(rwVENDORID_CRITERIONINT, 0x0E),

    rwID_ERRORMODULE = MAKECHUNKID(rwVENDORID_CRITERIONINT, 0x0F),

    rwID_METRICSMODULE = MAKECHUNKID(rwVENDORID_CRITERIONINT, 0x10),

    rwID_DRIVERMODULE = MAKECHUNKID(rwVENDORID_CRITERIONINT, 0x11),

    rwID_CHUNKGROUPMODULE = MAKECHUNKID(rwVENDORID_CRITERIONINT, 0x12),
    rwCRITERIONCOREIDFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum RwCriterionCoreID RwCriterionCoreID;
enum RwMemoryHintFlag
{

    rwMEMHINTFLAG_RESIZABLE = 0x01000000,
    rwMEMHINTFLAG_MASK = 0xFF000000,
    rwMEMHINTFLAGFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum RwMemoryHintFlag RwMemoryHintFlag;
#define rwsprintf RWSRCGLOBAL(stringFuncs).vecSprintf
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

#define rwPLUGIN_ID rwID_COREPLUGIN
#define BLOCKSIZE(size) ( ((size) + 31) & ~31 )
#define CHKHEAP(heap) 
#define FLOG(str,num) 
#define SUPERBLOCKALIGNMENT 128U
#define SBAL (SUPERBLOCKALIGNMENT - 1)
extern RwBool HeapSuperBlockReset(rxHeapSuperBlockDescriptor * superBlock, rxHeapSuperBlockDescriptor * attach2SuperBlock, RxHeap * heap);
extern void * HeapAllocFrom(RxHeap * heap, rxHeapFreeBlock * freeEntry, RwUInt32 size);

/* TIER2: unvalidated authority=diffemu */
void * RxHeapAlloc(RxHeap * heap, RwUInt32 size)
{
    CHKHEAP(heap);

    FLOG("request", size);

    size = BLOCKSIZE(size);

    {
        RwUInt32         n = heap->entriesUsed;
        rxHeapFreeBlock *freeEntry =
            &heap->freeBlocks[heap->entriesUsed - 1];

        do
        {
            if (freeEntry->size >= size)
            {
                heap->dirty = TRUE;
                return (HeapAllocFrom(heap, freeEntry, size));
            }
        }
        while (freeEntry--, --n);

        /* HeapGrow(heap, size) with HeapSuperBlockCreate / HeapSuperBlockDestroy, static in p2heap.c, inlined in MFL.exe: a new super
         * block in front of the list, its first block header behind the descriptor; the "Heap resized" message is formatted but not
         * sent (RwDebugSendMessage is empty in the release build) */
        {
            RwUInt32            superBlockSize = size + 3 * sizeof(rxHeapBlockHeader);
            rxHeapSuperBlockDescriptor *superBlock;
            rxHeapBlockHeader  *blockHdr = (rxHeapBlockHeader *) NULL;
            RwChar              string[256];

            if (superBlockSize < heap->superBlockSize)
            {
                superBlockSize = heap->superBlockSize;
            }

            superBlock = (rxHeapSuperBlockDescriptor *)
                RwMalloc(sizeof(rxHeapSuperBlockDescriptor) + SBAL + superBlockSize,
                         rwMEMHINTDUR_GLOBAL | rwMEMHINTFLAG_RESIZABLE |
                         rwID_PIPEMODULE);
            if (superBlock != NULL)
            {
                superBlock->start = (void *)
                    ((((mfl_uptr) & superBlock[1]) + SBAL) & ~(mfl_uptr)SBAL);
                superBlock->size = superBlockSize;
                superBlock->next = (rxHeapSuperBlockDescriptor *) NULL;

                if (HeapSuperBlockReset(superBlock, heap->head, heap))
                {
                    rxHeapSuperBlockDescriptor *tempSuperBlock;
                    RwUInt32            newSize = 0;

                    superBlock->next = heap->head;
                    heap->head = superBlock;

                    tempSuperBlock = heap->head;
                    while (tempSuperBlock)
                    {
                        newSize += tempSuperBlock->size;
                        tempSuperBlock = tempSuperBlock->next;
                    }
                    rwsprintf(string, RWSTRING("Heap resized from %d to %d bytes"),
                              (newSize - superBlockSize), newSize);

                    blockHdr = &((rxHeapBlockHeader *) superBlock->start)[1];
                }
                else
                {
                    RwFree(superBlock);
                }
            }

            if (blockHdr != NULL)
            {
                heap->dirty = TRUE;
                return (HeapAllocFrom
                         (heap, blockHdr->freeEntry, size));
            }
        }
    }

    RWERROR((E_RW_NOMEM, size));

    return (NULL);
}
