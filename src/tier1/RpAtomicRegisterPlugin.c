/* compile: /G5 */
#include "mfl_tier2.h"
/*0062ec04*/ extern int g_rp_62ec04;
extern int __cdecl rwPluginRegistryAddPlugin(void *reg, int size, int id, void *ctor, void *dtor, void *copy);
/* Registers an atomic plugin (size, id, constructor, destructor, copy callback); returns its offset. */
int RpAtomicRegisterPlugin(int size, int id, void *ctor, void *dtor, void *copy)
{
  return rwPluginRegistryAddPlugin(&g_rp_62ec04, size, id, ctor, dtor, copy);
}
