/* compile: /G5 */
#include "mfl_tier2.h"
extern void (__cdecl *g_rx_7d53c4)(void);
#define FLD(p, T, off) (*(T *)((char *)(p) + (off)))
/* Sets the three stream callbacks of the plugin with the given id; returns its offset or -1. */
int rwPluginRegistryAddPluginStream(void *reg, int id, void *a, void *b, void *c)
{
  char *e;
  for (e = FLD(reg, char *, 0x10); e; e = FLD(e, char *, 0x30)) if (FLD(e, int, 8) == id) { FLD(e, void *, 0xc) = a; FLD(e, void *, 0x10) = b; FLD(e, void *, 0x14) = c; return FLD(e, int, 0); }
  return -1;
}
