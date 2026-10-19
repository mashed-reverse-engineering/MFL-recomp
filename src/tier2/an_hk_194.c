#include "mfl_tier2.h"
/* FUN_004b10c0 */

/*
 * Bounded copy of a zero-terminated 16-bit string (inferred), like strncpy but
 * always terminated: copies at most buf_len - 1 characters from src to dest,
 * and writes a 0 at dest[buf_len - 1] if the source is longer.
 * dest:    destination buffer, must hold buf_len characters.
 * src:     zero-terminated source string.
 * buf_len: size of dest in characters; 0 copies nothing.
 * Returns: dest (the original start of the destination buffer).
 */
short * __stdcall FUN_004b10c0(short *dest, short *src, int buf_len)

{
  short ch;
  short *dest_start;

  dest_start = dest;
  if (buf_len != 0) {
    /* copy until the source terminator or the last slot is reached */
    while (buf_len = buf_len + -1, buf_len != 0) {
      ch = *src;
      *dest = ch;
      dest = dest + 1;
      src = src + 1;
      if (ch == 0) {
        return dest_start;
      }
    }
    /* buffer full: terminate the copy */
    *dest = 0;
  }
  return dest_start;
}


