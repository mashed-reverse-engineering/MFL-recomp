#include "mfl_tier2.h"
/* FUN_004b1090 */
/*
 * Copies a zero-terminated array of 16-bit characters (wide string) from src
 * to dest, including the terminating zero (inferred: strcpy for shorts).
 * dest must be large enough for the whole string.
 * Returns dest (the original dest pointer).
 */
short * __stdcall FUN_004b1090(short *dest, short *src)
{
  short ch;
  short *dest_start;

  dest_start = dest;
  /* copy one element at a time until the terminating zero has been copied */
  do {
    ch = *src;
    *dest = ch;
    dest = dest + 1;
    src = src + 1;
  } while (ch != 0);
  return dest_start;
}
