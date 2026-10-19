#include "mfl_tier2.h"

/* Rounds a byte offset up to a 16-byte boundary: (x + ALIGN16_PAD) & ALIGN16_MASK */
#define ALIGN16_PAD (0xf)
#define ALIGN16_MASK (0xfffffff0)

/* FUN_00570e80
 * Carves six 16-byte aligned sub-blocks out of a shared buffer and records where each one starts (inferred).
 *   alloc_state: running allocation cursor; the next free offset is stored at +8.
 *   out_offsets: receives the aligned start offset of each sub-block at +0x10 .. +0x24.
 *   layout:      source record; +0x3c is a count (slot_count = count + 1, inferred), +8 is a size (inferred).
 * Returns nothing; the results are in alloc_state (new cursor) and out_offsets (block starts).
 */

void FUN_00570e80(char *alloc_state,char *out_offsets,char *layout)

{
  int slot_count;
  uint aligned_offset;

  /* Block 0 starts at the cursor, rounded up to 16 bytes */
  aligned_offset = *(int *)(alloc_state + 8) + 0xfU & ALIGN16_MASK;
  slot_count = *(int *)(layout + 0x3c);
  *(uint *)(alloc_state + 8) = aligned_offset;
  *(uint *)(out_offsets + 0x10) = aligned_offset;
  slot_count = slot_count + 1;

  /* Blocks 1-5: each is sized from slot_count (strides 4, 12, 16, 32, 4 bytes) */
  aligned_offset = *(int *)(alloc_state + 8) + ALIGN16_PAD + slot_count * 4 & ALIGN16_MASK;
  *(uint *)(alloc_state + 8) = aligned_offset;
  *(uint *)(out_offsets + 0x14) = aligned_offset;
  aligned_offset = *(int *)(alloc_state + 8) + ALIGN16_PAD + slot_count * 0xc & ALIGN16_MASK;
  *(uint *)(alloc_state + 8) = aligned_offset;
  *(uint *)(out_offsets + 0x18) = aligned_offset;
  aligned_offset = slot_count * 0x10 + ALIGN16_PAD + *(int *)(alloc_state + 8) & ALIGN16_MASK;
  *(uint *)(alloc_state + 8) = aligned_offset;
  *(uint *)(out_offsets + 0x1c) = aligned_offset;
  aligned_offset = slot_count * 0x20 + ALIGN16_PAD + *(int *)(alloc_state + 8) & ALIGN16_MASK;
  *(uint *)(alloc_state + 8) = aligned_offset;
  *(uint *)(out_offsets + 0x20) = aligned_offset;
  aligned_offset = *(int *)(alloc_state + 8) + ALIGN16_PAD + slot_count * 4 & ALIGN16_MASK;
  *(uint *)(alloc_state + 8) = aligned_offset;
  *(uint *)(out_offsets + 0x24) = aligned_offset;
  /* Tail: advance the cursor past a block of 2 * layout size (inferred), kept 16-byte aligned */
  *(uint *)(alloc_state + 8) = *(int *)(alloc_state + 8) + ALIGN16_PAD + *(int *)(layout + 8) * 2 & ALIGN16_MASK;
  return;
}


