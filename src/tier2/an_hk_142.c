#include "mfl_tier2.h"
/* FUN_005bcea0 */

/* Bits tested in the source flags byte (src + 16). */
#define SRC_FLAG_BIT0 (1)
#define SRC_FLAG_BIT2 (4)

/*
 * Copies the state record at src into dst (field meanings inferred from the
 * access pattern; the struct is not in ghidra_types.h).
 * clear_dst != 0: zero the dst fields first, then copy.
 * Returns nothing; dst is the output.
 */
void FUN_005bcea0(undefined4 *src,undefined4 *dst,int clear_dst)

{
  undefined1 src_byte_5;

  /* clear_dst: reset every dst field before copying. */
  if (clear_dst != 0) {
    *dst = 0;
    dst[1] = 0;
    dst[2] = 0;
    *(undefined1 *)(dst + 3) = 0;
    *(undefined1 *)((char *)dst + 0xd) = 0;
    *(undefined1 *)(dst + 6) = 0;
    *(undefined1 *)((char *)dst + 0x19) = 0;
    dst[4] = 0;
    dst[5] = 0;
  }
  /* Copy: src byte 4 -> dst byte 12, src dword 0 -> dst dword 1, src byte 5 -> dst byte 13 (inferred). */
  *(undefined1 *)(dst + 3) = *(undefined1 *)(src + 1);
  dst[1] = *src;
  src_byte_5 = *(undefined1 *)((char *)src + 5);
  *(undefined1 *)(dst + 6) = 0;
  *(undefined1 *)((char *)dst + 0xd) = src_byte_5;
  /* dst byte 24 = flags from src byte 16: bit 0 -> 1, bit 2 -> 4 (inferred). */
  if ((*(byte *)(src + 4) & SRC_FLAG_BIT0) != 0) {
    *(undefined1 *)(dst + 6) = 1;
  }
  if ((*(byte *)(src + 4) & SRC_FLAG_BIT2) != 0) {
    *(byte *)(dst + 6) = *(byte *)(dst + 6) | 4;
  }
  return;
}


