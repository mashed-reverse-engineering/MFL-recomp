/* compile: /G5 */
#include "mfl_tier2.h"
extern void (__cdecl *g_rx_7d53c4)(void);
#define FLD(p, T, off) (*(T *)((char *)(p) + (off)))
/* Calls the copy callback of every plugin of a registry; returns the registry. */
void *rwPluginRegistryCopyObject(void *reg, void *dst, void *src)
{
  char *e;
  for (e = FLD(reg, char *, 0x10); e; e = FLD(e, char *, 0x30)) (*(void (__cdecl **)(void *, void *, int, int))(e + 0x28))(dst, src, FLD(e, int, 0), FLD(e, int, 4));
  return reg;
}
