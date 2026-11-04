/* compile: /G5 */
/* _rwResHeapFree (MFL.exe 0x004f3110): RenderWare Graphics 3.7 (Criterion Software), codfh name
 * Source: codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32 (src/rw/src/plcore/resmem.c, function _rwResHeapFree), a byte-identical decompilation of the RenderWare
 * build of an Xbox game; the Win32 branches it kept are RenderWare 3.7.2 source. Used with credit; split into one file per
 * function, the declarations it needs copied from the codfh headers named in the comments (PC build: no _XBOX, no RWDEBUG).
 * Changes: no renames; data definitions of the unit are extern declarations here.
 * Byte-identical to MFL.exe 0x004f3110 (tools/cmpfn.py /O2 /G5, relocations masked). */
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <math.h>
#include <stdarg.h>
typedef int  RwInt32;
typedef unsigned int RwUInt32;
#define RWFORCEENUMSIZEINT ((RwInt32)((~((RwUInt32)0))>>1))
#define _RWSTRING(x) x
#define RWSTRING(x) _RWSTRING(x)
#define RWRETURNVOID() return
#define RWFUNCTION(name)       /* No op */
#define RWASSERT(condition)    /* No op */
extern void     _rwResHeapFree(void *memory);
enum rwResHeapBlockFlags
{
    rwRESHEAP_BLOCKUSED = 1,    /* Block is in use */

    rwRESHEAPFLAGFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum rwResHeapBlockFlags rwResHeapBlockFlags;
typedef struct rwResHeapBlockHeader rwResHeapBlockHeader;
typedef struct rwResHeapHeader rwResHeapHeader;
struct rwResHeapHeader
{
    rwResHeapBlockHeader *firstBlock; /* First block in heap list */
    rwResHeapBlockHeader *firstFreeBlock; /* First free block */
};
struct rwResHeapBlockHeader
{
    rwResHeapHeader    *heap;   /* Pointer to heap header */
    rwResHeapBlockHeader *next; /* Pointer to next block in heap list */
    rwResHeapBlockHeader *prev; /* Pointer to prev block in heap list */
    RwUInt32            size;   /* Size of this heap block */
    rwResHeapBlockFlags flags;  /* Heap block flags */
    RwUInt32            pad[3]; /* Align this structure to 32 bytes (codfh-match: 3 in the image; 0x20-byte headers) */
};

/******************************************************************************
 *
 *  _rwResHeapFree()
 *
 *  Free block at <memory>. This will merge the free block with its neighbours
 *  if they are unused.
 */
void
_rwResHeapFree(void *memory)
{
    rwResHeapBlockHeader *block =
        (((rwResHeapBlockHeader *) memory) - 1);
    rwResHeapBlockHeader *prevBlock;
    rwResHeapBlockHeader *nextBlock;

    RWFUNCTION(RWSTRING("_rwResHeapFree"));
    RWASSERT(memory);
    RWASSERT(block->flags & rwRESHEAP_BLOCKUSED);
    RWASSERT(block->heap);
    RWASSERT((block == block->heap->firstBlock)
             || (block == block->prev->next));



    /* This block has been freed */
    block->flags = (rwResHeapBlockFlags) 0;

    /* Find previous and next (for merging blocks) */
    prevBlock = block->prev;
    nextBlock = block->next;

    /* Check if this is now the first free block */
    if (!block->heap->firstFreeBlock
        || (block < block->heap->firstFreeBlock))
    {
        block->heap->firstFreeBlock = block;
    }

    /* Try and merge with previous block (if there is one) */
    if (prevBlock && (~prevBlock->flags & rwRESHEAP_BLOCKUSED))
    {
        /* Merge it */
        prevBlock->next = nextBlock;
        if (nextBlock)
        {
            nextBlock->prev = prevBlock;
        }

        /* Adjust the size */
        prevBlock->size += (block->size + sizeof(rwResHeapBlockHeader));

        /* The current block now becomes the previous block */
        block = prevBlock;
    }


    /* try and merge with next block (if there is one) */
    if (nextBlock && (~nextBlock->flags & rwRESHEAP_BLOCKUSED))
    {
        /* Merge it */
        block->next = nextBlock->next;
        if (nextBlock->next)
        {
            nextBlock->next->prev = block;
        }

        /* Adjust the size */
        block->size += (nextBlock->size + sizeof(rwResHeapBlockHeader));
    }


    RWRETURNVOID();
}
