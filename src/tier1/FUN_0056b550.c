/* compile: /G5 */
#include "mfl_tier2.h"
typedef void *(__cdecl *IterFn)(void *, void *);
#define FLD(p, T, off) (*(T *)((char *)(p) + (off)))
/* Calls a callback for each non-NULL item of the array at +0x50 (count at +0x44) until it returns something other than the item; returns the container. (Rwp library, proposed name rwp_volume_0056b550) */
void *FUN_0056b550(void *t, IterFn cb, void *data)
{
  unsigned i;
  for (i = 0; i < FLD(t, unsigned, 0x44); i++) { void *it = FLD(t, void **, 0x50)[i]; if (it) if (cb(it, data) != it) break; }
  return t;
}
