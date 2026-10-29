/* compile: /G5 */
#include "mfl_tier2.h"
#define FLD(p, T, off) (*(T *)((char *)(p) + (off)))
/* libpng: sets the tRNS-to-alpha transformation flag. */
void png_set_tRNS_to_alpha(void *p)
{
  FLD(p, unsigned, 0x70) |= 0x1000;
}
