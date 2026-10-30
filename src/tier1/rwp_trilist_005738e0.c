/* compile: /G5 */
#include "mfl_tier2.h"
extern int g_rwp_trilist_sector_plugin_offset;
extern int RpWorldSectorRegisterPlugin(int size, int id, void *ctor, void *dtor, void *copy);
extern int RpWorldSectorRegisterPluginStream(int id, void *read, void *write, void *get_size);
extern void rpD3D9UsageFlagDest();
extern void FUN_0053eac0();
extern void LAB_00573940();
extern void LAB_00573970();
extern void LAB_005739e0();
/* Registers the trilist world sector plugin (id 0x902) and its stream callbacks once; returns whether the plugin offset is valid. (Rwp library) */
int rwp_trilist_005738e0(void)
{
  if (g_rwp_trilist_sector_plugin_offset != -1) return 0;
  g_rwp_trilist_sector_plugin_offset = RpWorldSectorRegisterPlugin(4, 0x902, rpD3D9UsageFlagDest, FUN_0053eac0, FUN_0053eac0);
  RpWorldSectorRegisterPluginStream(0x902, LAB_00573970, LAB_00573940, LAB_005739e0);
  return g_rwp_trilist_sector_plugin_offset != -1;
}
