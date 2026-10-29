/* compile: /G5 */
#include "mfl_tier2.h"
typedef void *(__cdecl *IterFn)(void *, void *);
#define FLD(p, T, off) (*(T *)((char *)(p) + (off)))
/* Calls a callback for each non-NULL item of the array at +8 (count at +0xc) until it returns zero; returns the container. (Rwp library, proposed name rwp_obj_0056e400) */
void *FUN_0056e400(void *t, IterFn cb, void *data)
{
  unsigned i;
  for (i = 0; i < FLD(t, unsigned, 0xc); i++) { void *it = FLD(t, void **, 8)[i]; if (it) if (!cb(it, data)) break; }
  return t;
}
