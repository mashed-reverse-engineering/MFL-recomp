/* compile: /G5 */
#include "mfl_tier2.h"
/*007da6bc*/ extern int g_ptank_plugin_offset;
#define FLD(p, T, off) (*(T *)((char *)(p) + (off)))
/* Unlocks a point tank: ORs the lock flags into the dirty flags and clears them; returns the atomic. */
void *RpPTankAtomicUnlockImpl(void *a)
{
  char *p = FLD(a, char *, g_ptank_plugin_offset);
  unsigned n = FLD(p, unsigned, 0x3c);
  unsigned m = FLD(p, unsigned, 0x40);
  FLD(p, int, 0x3c) = 0;
  FLD(p, unsigned, 0x40) = m | n;
  return a;
}
