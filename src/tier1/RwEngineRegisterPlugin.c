/* compile: /G5 */
#include "mfl_tier2.h"
/*0062e578*/ extern int g_rw_62e578;
extern int __cdecl rwPluginRegistryAddPlugin(void *reg, int size, int id, void *ctor, void *dtor, void *copy);
/* Registers an engine plugin (size, id, constructor, destructor; no copy callback); returns its offset. */
int RwEngineRegisterPlugin(int size, int id, void *ctor, void *dtor)
{
  return rwPluginRegistryAddPlugin(&g_rw_62e578, size, id, ctor, dtor, 0);
}
