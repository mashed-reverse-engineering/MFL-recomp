/* compile: /G5 */
/* _rwResHeapInit (MFL.exe 0x004f30c0): RenderWare Graphics 3.7 (Criterion Software), codfh name
 * Source: codfh-win32 by L2KSolkov, https://github.com/L2KSolkov/codfh-win32 (src/rw/src/plcore/resmem.c, function _rwResHeapInit), a byte-identical decompilation of the RenderWare
 * build of an Xbox game; the Win32 branches it kept are RenderWare 3.7.2 source. Used with credit; split into one file per
 * function, the declarations it needs copied from the codfh headers named in the comments (PC build: no _XBOX, no RWDEBUG).
 * Changes: no renames; data definitions of the unit are extern declarations here.
 * Byte-identical to MFL.exe 0x004f30c0 (tools/cmpfn.py /O2 /G5, relocations masked). */
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <math.h>
#include <stdarg.h>
typedef int  RwInt32;
typedef unsigned int RwUInt32;
typedef RwInt32 RwBool;
#define RWFORCEENUMSIZEINT ((RwInt32)((~((RwUInt32)0))>>1))
#define FALSE 0
#define TRUE !FALSE
#define _RWSTRING(x) x
#define RWSTRING(x) _RWSTRING(x)
#define RWRETURN(value) return(value)
#define RWFUNCTION(name)       /* No op */
#define RWASSERT(condition)    /* No op */
extern RwBool   _rwResHeapInit(void *resHeap, RwUInt32 size);
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
 *  _rwResHeapInit()
 *
 *  Initialise the block of memory at resHeap, of the given size, for heap
 *  management. Returns NULL if size is not sufficient.
 */
RwBool
_rwResHeapInit(void *resHeap, RwUInt32 size)
{
    rwResHeapHeader    *heapInfo = (rwResHeapHeader *) resHeap;
    rwResHeapBlockHeader *firstBlock;
    RwUInt32            start, end;
    RwInt32             blockSize;

    RWFUNCTION(RWSTRING("_rwResHeapInit"));
    RWASSERT(resHeap);
    RWASSERT(size);


    /* Get 32 byte aligned start and end of the heap block */
    start =
        ((RwUInt32) resHeap + sizeof(rwResHeapHeader) +
         31) & 0xFFFFFFE0;
    end = ((RwUInt32) resHeap + size) & 0xFFFFFFE0;

    /* Get size of available storage */
    blockSize = end - start - sizeof(rwResHeapBlockHeader);
    if (blockSize < 32)
    {
        RWRETURN(FALSE);
    }

    /* Initialize the first block header */
    firstBlock = (rwResHeapBlockHeader *) start;
    firstBlock->heap = heapInfo; /* Link back to heap header */
    firstBlock->next = (rwResHeapBlockHeader *) NULL;
    firstBlock->prev = (rwResHeapBlockHeader *) NULL;
    firstBlock->flags = (rwResHeapBlockFlags) 0;
    firstBlock->size = blockSize;


    /* Initialize heap info */
    heapInfo->firstBlock = firstBlock;
    heapInfo->firstFreeBlock = firstBlock;


    RWRETURN(TRUE);
}
