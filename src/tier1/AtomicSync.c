/* compile: /G5 */
#include "mfl_tier2.h"
extern void __cdecl rpAtomicResyncInterpolatedSphere(void *a);
#define FLD(p, T, off) (*(T *)((char *)(p) + (off)))
/* Resynchronises the interpolated sphere when flagged and marks the atomic dirty; returns the atomic. */
void *AtomicSync(void *a)
{
  if (FLD(a, unsigned char, 0x4c) & 2) rpAtomicResyncInterpolatedSphere(a);
  FLD(a, unsigned char, 3) |= 1;
  return a;
}
