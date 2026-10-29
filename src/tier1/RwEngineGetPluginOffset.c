/* compile: /G5 */
#include "mfl_tier2.h"
/*0062e578*/ extern int g_rw_62e578[];
/*004e9aa0*/ extern int __cdecl rwPluginRegistryGetPluginOffset(void *reg, int id);
/* Offset of a plugin in the engine plugin registry, or -1. */
int RwEngineGetPluginOffset(int id)
{
  return rwPluginRegistryGetPluginOffset(g_rw_62e578, id);
}
