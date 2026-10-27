#include "mfl_tier2.h"
/*00629468*/ extern undefined4 g_gfx_629468;
/*007da6bc*/ extern undefined4 g_ptank_plugin_offset;


/* particle_system_set_tint */

/* "stores 1 colour (default 0x629468) into the ptank plugin and sets flag 0x2000" */

void __cdecl particle_system_set_tint(char *param_1,undefined4 *param_2)
{
  uint *puVar1;
  int iVar2;
  
  if (param_2 == (undefined4 *)0x0) {
    param_2 = &g_gfx_629468;
  }
  *(undefined4 *)(*(char **)(*(char **)(param_1 + 4) + g_ptank_plugin_offset) + 0xc4) = *param_2;
  iVar2 = **(int **)(*(char **)(*(char **)(param_1 + 4) + 0x18) + 0x20);
  if (iVar2 != 0) {
    *(undefined4 *)(iVar2 + 4) =
         *(undefined4 *)(*(char **)(*(char **)(param_1 + 4) + g_ptank_plugin_offset) + 0xc4);
  }
  *(uint *)(*(char **)(*(char **)(param_1 + 4) + g_ptank_plugin_offset) + 0x40) |= 0x2000;
  return;
}
