/* compile: /G5 */
#include "mfl_tier2.h"
#define FLD(p, T, off) (*(T *)((char *)(p) + (off)))
/* Sets the rights callback of the plugin with the given id; returns its offset or -1. */
int rwPluginRegistryAddPlgnStrmRightsCB(void *reg, int id, void *cb)
{
  char *e;
  for (e = FLD(reg, char *, 0x10); e; e = FLD(e, char *, 0x30)) if (FLD(e, int, 8) == id) { FLD(e, void *, 0x1c) = cb; return FLD(e, int, 0); }
  return -1;
}
