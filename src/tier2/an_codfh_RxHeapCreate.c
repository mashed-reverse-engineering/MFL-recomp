/* RxHeapCreate (MFL.exe 004f1900). Adapted from codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32, src/rw/src/pipe/p2/p2heap.c @e578672
 *, not byte-identical, changes: cut down to this function and the private declarations it uses,
 * HeapSuperBlockCreate / HeapSuperBlockDestroy and rxHeapReset inlined by hand (MFL.exe has them inlined), the alignment cast through
 * mfl_uptr, release build macros expanded (RWFUNCTION / RWASSERT dropped, RWRETURN -> return), conditional compilation resolved for the
 * release D3D9 build, the SDK includes replaced by rw37_slim.h, data and functions MFL.exe names declared extern at their addresses.
 * Generated, then hand-checked; compared with MFL.exe by differential emulation (tier 2). */
#include "rw37_slim.h"
/* ---- SDK declarations only this row uses (generated together with rw37_slim.h: do not edit between the markers) ---- */
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

#define MINHEAPSIZE 1024
#define BLOCKSIZE(size) ( ((size) + 31) & ~31 )
#define SUPERBLOCKALIGNMENT 128U
#define SBAL (SUPERBLOCKALIGNMENT - 1)
extern RwBool HeapSuperBlockReset(rxHeapSuperBlockDescriptor * superBlock, rxHeapSuperBlockDescriptor * attach2SuperBlock, RxHeap * heap);

/* TIER2: unvalidated authority=diffemu */
RxHeap * RxHeapCreate(RwUInt32 size)
{
    RxHeap *heap;

    if (size < MINHEAPSIZE) size = MINHEAPSIZE;

    heap = (RxHeap *) RwMalloc(sizeof(RxHeap), rwMEMHINTDUR_GLOBAL |
                               rwID_PIPEMODULE);

    if (heap != NULL)
    {
        rxHeapSuperBlockDescriptor *superBlock;

        size = BLOCKSIZE(size);

        if (size < (BLOCKSIZE(1) + 3 * sizeof(rxHeapBlockHeader)))
        {
            size = BLOCKSIZE(1) + 3 * sizeof(rxHeapBlockHeader);
        }

        /* HeapSuperBlockCreate(size), static in p2heap.c, inlined in MFL.exe */
        superBlock = (rxHeapSuperBlockDescriptor *)
            RwMalloc(sizeof(rxHeapSuperBlockDescriptor) + SBAL + size,
                     rwMEMHINTDUR_GLOBAL | rwMEMHINTFLAG_RESIZABLE |
                     rwID_PIPEMODULE);

        if (superBlock != NULL)
        {
            rxHeapSuperBlockDescriptor *prev = (rxHeapSuperBlockDescriptor *) NULL;
            rxHeapSuperBlockDescriptor *iter;
            RwBool              ok = TRUE;

            superBlock->start = (void *)
                ((((mfl_uptr) & superBlock[1]) + SBAL) & ~(mfl_uptr)SBAL);
            superBlock->size = size;
            superBlock->next = (rxHeapSuperBlockDescriptor *) NULL;

            heap->superBlockSize = size;
            heap->head = superBlock;
            heap->freeBlocks = (rxHeapFreeBlock *) NULL;
            heap->entriesAlloced = 0;
            heap->entriesUsed = 0;

            heap->dirty = TRUE;

            /* RxHeapReset(heap) = rxHeapReset (MFL.exe 0x4f1830), inlined here in MFL.exe */
            heap->entriesUsed = 0;
            for (iter = heap->head->next; iter != NULL; iter = iter->next)
            {
                if (!HeapSuperBlockReset(iter, prev, heap))
                {
                    ok = FALSE;
                    break;
                }
                if (prev == NULL)
                {
                    heap->headBlock = (rxHeapBlockHeader *) iter->start;
                }
                prev = iter;
            }
            if (ok)
            {
                iter = heap->head;
                if (HeapSuperBlockReset(iter, prev, heap))
                {
                    if (prev == NULL)
                    {
                        heap->headBlock = (rxHeapBlockHeader *) iter->start;
                    }
                    heap->dirty = FALSE;
                    return (heap);
                }
            }

            /* HeapSuperBlockDestroy(superBlock) */
            RwFree(superBlock);
        }

        RwFree(heap);
    }

    return ((RxHeap *) NULL);
}
