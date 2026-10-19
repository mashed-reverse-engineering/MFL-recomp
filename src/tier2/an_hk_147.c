#include "mfl_tier2.h"
#define HEAP_HEAD_OFFSET (0x8)
#define HEAP_TAIL_OFFSET (0xc)
#define BLOCK_HEADER_SIZE (0xc)
/* FUN_005be8e0 */
/* Allocates `size` bytes aligned to `alignment` (inferred: power of two) from the
   doubly linked block list in `heap` (inferred). Each block is a BLOCK_HEADER_SIZE
   header (next at +0, prev at +4, size at +8) followed by its data.
   A new block is carved from the gap after the first block that leaves room for
   a header plus `size`, and linked in right after that block.
   Returns the new block's data pointer, or 0 if no gap fits (out of memory). */

uint FUN_005be8e0(char *heap,int size,int alignment)

{
  uint *next_block;
  uint block_size;
  char *aligned_data;
  uint *new_block;
  uint *block;

  /* Start at the list head and walk forward until a gap fits */
  block = *(uint **)(heap + HEAP_HEAD_OFFSET);
  do {
    next_block = (uint *)*block;
    block_size = block[2];
    /* Gap between this block's end and next_block must hold a header plus size */
    if (size + 0xcU <= (LONG_PTR)next_block + (-(LONG_PTR)block - block_size) + -BLOCK_HEADER_SIZE) {
      /* Candidate data pointer just past this block and a header, rounded up to alignment */
      aligned_data = (int)block + alignment + block_size + 0x17 & ~(alignment - 1U);
      new_block = (uint *)(aligned_data - BLOCK_HEADER_SIZE);
      /* The new block must fit inside the gap between this block and next_block */
      if ((size + 0xcU + (LONG_PTR)new_block <= next_block) &&
         ((uint *)((char *)block + block_size + BLOCK_HEADER_SIZE) <= new_block)) {
        if (new_block == (uint *)0x0) {
          return 0;
        }
        /* block_size is reused here to hold the old next link */
        block_size = *block;
        /* Link new_block in: new->prev = block, new->next = old next */
        *(uint **)(aligned_data - 8) = block;
        *new_block = block_size;
        /* Fix the old next's prev, unless it is the tail sentinel */
        if (block_size != *(uint *)(heap + HEAP_TAIL_OFFSET)) {
          *(uint **)(block_size + 4) = new_block;
        }
        /* Fix block's next, unless block is the tail sentinel (inferred) */
        if (*(undefined4 **)(aligned_data - 8) != *(undefined4 **)(heap + HEAP_TAIL_OFFSET)) {
          **(undefined4 **)(aligned_data - 8) = new_block;
        }
        *(int *)(aligned_data - 4) = size;
        return aligned_data;
      }
    }
    /* No fit after this block: advance, and give up at the tail sentinel */
    block = next_block;
    if (next_block == (uint *)*(uint *)(heap + HEAP_TAIL_OFFSET)) {
      return 0;
    }
  } while( true );
}


