/* compile: /G5 */
#include "mfl_tier2.h"
/*0062ec70*/ extern int g_rp_62ec70;
extern int __cdecl rwPluginRegistryAddPlugin(void *reg, int size, int id, void *ctor, void *dtor, void *copy);
/* Registers a geometry plugin (size, id, constructor, destructor, copy callback); returns its offset. */
int RpGeometryRegisterPlugin(int size, int id, void *ctor, void *dtor, void *copy)
{
  return rwPluginRegistryAddPlugin(&g_rp_62ec70, size, id, ctor, dtor, copy);
}
