/* compile: /G5 */
#include "mfl_tier2.h"
#define FLD(p, T, off) (*(T *)((char *)(p) + (off)))
/* Offset of the plugin with the given id in a plugin registry, or -1. */
int rwPluginRegistryGetPluginOffset(void *reg, int id)
{
  char *e;
  for (e = FLD(reg, char *, 0x10); e; e = FLD(e, char *, 0x30)) if (FLD(e, int, 8) == id) return FLD(e, int, 0);
  return -1;
}
