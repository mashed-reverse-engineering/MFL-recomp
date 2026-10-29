/* compile: /G5 */
#include "mfl_tier2.h"
#define FLD(p, T, off) (*(T *)((char *)(p) + (off)))
/* libpng: stores the error pointer, error callback and warning callback. */
void png_set_error_fn(void *p, void *err, void *e, void *w)
{
  FLD(p, void *, 0x48) = err;
  FLD(p, void *, 0x40) = e;
  FLD(p, void *, 0x44) = w;
}
