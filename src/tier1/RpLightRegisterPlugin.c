/* compile: /G5 */
#include "mfl_tier2.h"
/*0062ebcc*/ extern int g_rp_62ebcc;
extern int __cdecl rwPluginRegistryAddPlugin(void *reg, int size, int id, void *ctor, void *dtor, void *copy);
/* Registers a light plugin (size, id, constructor, destructor, copy callback); returns its offset. */
int RpLightRegisterPlugin(int size, int id, void *ctor, void *dtor, void *copy)
{
  return rwPluginRegistryAddPlugin(&g_rp_62ebcc, size, id, ctor, dtor, copy);
}
