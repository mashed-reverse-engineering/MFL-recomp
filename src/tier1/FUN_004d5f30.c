/* compile: /G5 */
#include "mfl_tier2.h"
/*0062e6d0*/ extern int g_rw_62e6d0;
extern int __cdecl rwPluginRegistryAddPlugin(void *reg, int size, int id, void *ctor, void *dtor, void *copy);
/* Registers a plugin in the texture dictionary registry; returns its offset. */
int FUN_004d5f30(int size, int id, void *ctor, void *dtor, void *copy)
{
  return rwPluginRegistryAddPlugin(&g_rw_62e6d0, size, id, ctor, dtor, copy);
}
