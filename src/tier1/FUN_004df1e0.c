/* compile: /G5 */
#include "mfl_tier2.h"
/*0062e6d0*/ extern int g_rw_62e6d0;
/*004f1e00*/ extern int __cdecl rwPluginRegistryAddPluginStream(void *reg, int id, void *rd, void *wr, void *sz);
/* Registers the stream callbacks of a plugin in the texture dictionary registry. */
int FUN_004df1e0(int id, void *rd, void *wr, void *sz)
{
  return rwPluginRegistryAddPluginStream(&g_rw_62e6d0, id, rd, wr, sz);
}
