#include "mfl_tier2.h"

#define SIZE_WORD (2)
#define SIZE_DWORD (4)

/* output position: the same word viewed as a byte, word or dword write pointer */
typedef struct OutCursor {
  union {
    undefined1 *byte_ptr;
    undefined2 *word_ptr;
    uint *dword_ptr;
  } pos;
} OutCursor;

/* FUN_005bf3e0 (inferred: writes a 1, 2 or 4 byte value into an output buffer, byte-swapped
 * to big-endian for 2 and 4 bytes; any other size writes nothing).
 *
 *   out_cursor  pointer to the output position; advanced past the bytes written
 *   src         pointer to the source value (its low `size` bytes are read)
 *   size        number of bytes to write: 1, SIZE_WORD or SIZE_DWORD
 */
void FUN_005bf3e0(OutCursor *out_cursor,uint *src,int size)

{
  uint value;
  uint *dst32;
  undefined2 *dst16;
  undefined1 *dst8;

  /* 1 byte: copy the low byte as is */
  if (size == 1) {
    dst8 = out_cursor->pos.byte_ptr;
    *dst8 = (char)*src;
    out_cursor->pos.byte_ptr = dst8 + 1;
    return;
  }
  if (size != SIZE_WORD) {
    /* 4 bytes: byte-swap the value (inferred from the mask/shift expression) and store it */
    if (size == SIZE_DWORD) {
      value = *src;
      dst32 = out_cursor->pos.dword_ptr;
      *dst32 = (value << 0x10 | value & 0xff00 | value >> 0x10 & 0xff) << 8 |
                (uint)*(byte *)((char *)src + 3);
      out_cursor->pos.dword_ptr = dst32 + 1;
    }
    return;
  }
  /* 2 bytes: byte-swap the low two bytes and store them */
  dst16 = out_cursor->pos.word_ptr;
  *dst16 = CONCAT11((char)*src,*(undefined1 *)((char *)src + 1));
  out_cursor->pos.word_ptr = dst16 + 1;
  return;
}
