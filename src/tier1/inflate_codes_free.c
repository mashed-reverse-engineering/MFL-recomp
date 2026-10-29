/* compile: /G5 */
#include "mfl_tier2.h"
#define FLD(p, T, off) (*(T *)((char *)(p) + (off)))
/* zlib: releases the codes state through the stream free callback. */
void inflate_codes_free(void *c, void *z)
{
  (*(void (__cdecl **)(void *, void *))((char *)z + 0x24))(FLD(z, void *, 0x28), c);
}
