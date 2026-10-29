/* compile: /G5 */
#include "mfl_tier2.h"
/*0062ec1c*/ extern int g_rp_62ec1c;
extern int __cdecl rwPluginRegistryAddPlugin(void *reg, int size, int id, void *ctor, void *dtor, void *copy);
/* Registers a plugin in the clump registry; returns its offset. */
int FUN_004f7eb0(int size, int id, void *ctor, void *dtor, void *copy)
{
  return rwPluginRegistryAddPlugin(&g_rp_62ec1c, size, id, ctor, dtor, copy);
}
