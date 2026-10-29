/* compile: /G5 */
#include "mfl_tier2.h"
/*0062e718*/ extern int g_rw_62e718;
extern int __cdecl rwPluginRegistryAddPlugin(void *reg, int size, int id, void *ctor, void *dtor, void *copy);
/* Registers a raster plugin (size, id, constructor, destructor, copy callback); returns its offset. */
int RwRasterRegisterPlugin(int size, int id, void *ctor, void *dtor, void *copy)
{
  return rwPluginRegistryAddPlugin(&g_rw_62e718, size, id, ctor, dtor, copy);
}
