/* compile: /G5 */
#include "mfl_tier2.h"
typedef void (__cdecl *PngFn)(void *);
#define FLD(p, T, off) (*(T *)((char *)(p) + (off)))
/* libpng: calls the output flush callback if one is set. */
void png_flush(void *p)
{
  PngFn f = FLD(p, PngFn, 0x14c);
  if (f) f(p);
}
