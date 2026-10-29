/* compile: /G5 */
#include "mfl_tier2.h"
/*0062ef88*/ extern int g_rp_62ef88;
/*004f1e00*/ extern int __cdecl rwPluginRegistryAddPluginStream(void *reg, int id, void *rd, void *wr, void *sz);
/* Registers the stream callbacks (read, write, get size) of a world sector plugin. */
int RpWorldSectorRegisterPluginStream(int id, void *rd, void *wr, void *sz)
{
  return rwPluginRegistryAddPluginStream(&g_rp_62ef88, id, rd, wr, sz);
}
