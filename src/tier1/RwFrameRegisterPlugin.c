/* compile: /G5 */
#include "mfl_tier2.h"
/*0062e514*/ extern int g_rw_62e514;
extern int __cdecl rwPluginRegistryAddPlugin(void *reg, int size, int id, void *ctor, void *dtor, void *copy);
/* Registers a frame plugin (size, id, constructor, destructor, copy callback); returns its offset. */
int RwFrameRegisterPlugin(int size, int id, void *ctor, void *dtor, void *copy)
{
  return rwPluginRegistryAddPlugin(&g_rw_62e514, size, id, ctor, dtor, copy);
}
