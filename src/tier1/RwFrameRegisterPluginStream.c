/* compile: /G5 */
#include "mfl_tier2.h"
/*0062e514*/ extern int g_rw_62e514;
/*004f1e00*/ extern int __cdecl rwPluginRegistryAddPluginStream(void *reg, int id, void *rd, void *wr, void *sz);
/* Registers the stream callbacks (read, write, get size) of a frame plugin. */
int RwFrameRegisterPluginStream(int id, void *rd, void *wr, void *sz)
{
  return rwPluginRegistryAddPluginStream(&g_rw_62e514, id, rd, wr, sz);
}
