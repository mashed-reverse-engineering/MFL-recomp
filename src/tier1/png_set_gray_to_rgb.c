/* compile: /G5 */
#include "mfl_tier2.h"
#define FLD(p, T, off) (*(T *)((char *)(p) + (off)))
/* libpng: sets the gray-to-RGB transformation flag. */
void png_set_gray_to_rgb(void *p)
{
  FLD(p, unsigned, 0x70) |= 0x4000;
}
