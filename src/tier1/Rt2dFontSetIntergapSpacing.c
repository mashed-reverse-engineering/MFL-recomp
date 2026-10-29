/* compile: /G5 */
#include "mfl_tier2.h"
#define FLD(p, T, off) (*(T *)((char *)(p) + (off)))
/* Sets the inter-glyph spacing of a 2D font. */
void Rt2dFontSetIntergapSpacing(void *f, float v)
{
  FLD(f, float, 0xc) = v;
}
