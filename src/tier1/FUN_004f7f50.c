/* compile: /G5 */
#include "mfl_tier2.h"
/*0062ec04*/ extern int g_rp_62ec04;
/*004e9aa0*/ extern int __cdecl rwPluginRegistryGetPluginOffset(void *reg, int id);
/* Offset of an atomic plugin id in the atomic plugin registry, or -1. */
int FUN_004f7f50(int id)
{
  return rwPluginRegistryGetPluginOffset(&g_rp_62ec04, id);
}
