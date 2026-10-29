/* compile: /G5 */
#include "mfl_tier2.h"
/*0062e534*/ extern int g_rw_62e534;
extern int __cdecl rwPluginRegistryAddPlugin(void *reg, int size, int id, void *ctor, void *dtor, void *copy);
/* Registers a camera plugin (size, id, constructor, destructor, copy callback); returns its offset. */
int RwCameraRegisterPlugin(int size, int id, void *ctor, void *dtor, void *copy)
{
  return rwPluginRegistryAddPlugin(&g_rw_62e534, size, id, ctor, dtor, copy);
}
