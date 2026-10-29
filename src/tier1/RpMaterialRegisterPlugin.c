/* compile: /G5 */
#include "mfl_tier2.h"
/*0062ec44*/ extern int g_rp_62ec44;
extern int __cdecl rwPluginRegistryAddPlugin(void *reg, int size, int id, void *ctor, void *dtor, void *copy);
/* Registers a material plugin (size, id, constructor, destructor, copy callback); returns its offset. */
int RpMaterialRegisterPlugin(int size, int id, void *ctor, void *dtor, void *copy)
{
  return rwPluginRegistryAddPlugin(&g_rp_62ec44, size, id, ctor, dtor, copy);
}
