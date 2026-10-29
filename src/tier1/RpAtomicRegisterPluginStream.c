/* compile: /G5 */
#include "mfl_tier2.h"
/*0062ec04*/ extern int g_rp_62ec04;
/*004f1e00*/ extern int __cdecl rwPluginRegistryAddPluginStream(void *reg, int id, void *rd, void *wr, void *sz);
/* Registers the stream callbacks (read, write, get size) of an atomic plugin. */
int RpAtomicRegisterPluginStream(int id, void *rd, void *wr, void *sz)
{
  return rwPluginRegistryAddPluginStream(&g_rp_62ec04, id, rd, wr, sz);
}
