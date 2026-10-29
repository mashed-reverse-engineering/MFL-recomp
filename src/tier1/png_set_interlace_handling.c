/* compile: /G5 */
#include "mfl_tier2.h"
#define FLD(p, T, off) (*(T *)((char *)(p) + (off)))
/* libpng: enables interlace handling when the image is interlaced; returns the number of passes (7 or 1). */
int png_set_interlace_handling(void *p)
{
  if (FLD(p, unsigned char, 0x123)) { FLD(p, unsigned, 0x70) |= 2; return 7; }
  return 1;
}
