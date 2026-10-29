/* compile: /G5 */
#include "mfl_tier2.h"
/*0062ebec*/ extern int g_rp_62ebec;
extern int __cdecl rwPluginRegistryAddPlugin(void *reg, int size, int id, void *ctor, void *dtor, void *copy);
/* Registers a world plugin (size, id, constructor, destructor, copy callback); returns its offset. */
int RpWorldRegisterPlugin(int size, int id, void *ctor, void *dtor, void *copy)
{
  return rwPluginRegistryAddPlugin(&g_rp_62ebec, size, id, ctor, dtor, copy);
}
