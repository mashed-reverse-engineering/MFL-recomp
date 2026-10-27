#include "mfl_tier2.h"
/*004c66a0*/ extern void * mem_zero(void * p0, int p1);
/*007d2128*/ extern struct RwGlobalsRelease *RwEngineInstance;
/* alloc_zeroed */
/*
 * Allocates a zero-filled array of count elements of size bytes each.
 * Memory comes from the RenderWare engine allocator (rwmalloc), then the whole
 * block is cleared with mem_zero. Returns the block (NULL on allocation failure, inferred).
 * Parameters: count = number of elements, size = bytes per element.
 */
void * alloc_zeroed(int count,int size)
{
  void *block;

  /* engine allocation; the 0 is the rwmalloc hint flags argument */
  block = RwMalloc(count * size,0);
  /* clear the whole block to zero */
  mem_zero(block,count * size);
  return block;
}
