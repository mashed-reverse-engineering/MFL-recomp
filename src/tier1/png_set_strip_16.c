/* compile: /G5 */
#include "mfl_tier2.h"
#define FLD(p, T, off) (*(T *)((char *)(p) + (off)))
/* libpng: sets the strip-16-bit transformation flag. */
void png_set_strip_16(void *p)
{
  FLD(p, unsigned, 0x70) |= 0x400;
}
