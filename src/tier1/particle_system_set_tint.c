#include "mfl_tier2.h"
/*00629468*/ extern undefined4 g_gfx_629468;
/*007da6bc*/ extern undefined4 g_ptank_plugin_offset;


/* particle_system_set_tint */

/* "stores 1 colour (default 0x629468) into the ptank plugin and sets flag 0x2000" */

void __cdecl particle_system_set_tint(char *system,undefined4 *tint)
{
  uint *unused_ptr;
  int plugin_obj;
  
  if (tint == (undefined4 *)0x0) {
    tint = &g_gfx_629468;
  }
  *(undefined4 *)(*(char **)(*(char **)(system + 4) + g_ptank_plugin_offset) + 0xc4) = *tint;
  plugin_obj = **(int **)(*(char **)(*(char **)(system + 4) + 0x18) + 0x20);
  if (plugin_obj != 0) {
    *(undefined4 *)(plugin_obj + 4) =
         *(undefined4 *)(*(char **)(*(char **)(system + 4) + g_ptank_plugin_offset) + 0xc4);
  }
  *(uint *)(*(char **)(*(char **)(system + 4) + g_ptank_plugin_offset) + 0x40) |= 0x2000;
  return;
}
