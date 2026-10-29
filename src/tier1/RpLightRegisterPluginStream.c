/* compile: /G5 */
#include "mfl_tier2.h"
/*0062ebcc*/ extern int g_rp_62ebcc;
/*004f1e00*/ extern int __cdecl rwPluginRegistryAddPluginStream(void *reg, int id, void *rd, void *wr, void *sz);
/* Registers the stream callbacks (read, write, get size) of a light plugin. */
int RpLightRegisterPluginStream(int id, void *rd, void *wr, void *sz)
{
  return rwPluginRegistryAddPluginStream(&g_rp_62ebcc, id, rd, wr, sz);
}
