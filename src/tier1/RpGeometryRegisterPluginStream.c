/* compile: /G5 */
#include "mfl_tier2.h"
/*0062ec70*/ extern int g_rp_62ec70;
/*004f1e00*/ extern int __cdecl rwPluginRegistryAddPluginStream(void *reg, int id, void *rd, void *wr, void *sz);
/* Registers the stream callbacks (read, write, get size) of a geometry plugin. */
int RpGeometryRegisterPluginStream(int id, void *rd, void *wr, void *sz)
{
  return rwPluginRegistryAddPluginStream(&g_rp_62ec70, id, rd, wr, sz);
}
