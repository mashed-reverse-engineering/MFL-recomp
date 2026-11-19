#include "mfl_tier2.h"
/*007da6bc*/ extern undefined4 g_ptank_plugin_offset;


/* particle_system_set_value_c0 */

/* "stores a value and sets flag 0x20000" */

void particle_system_set_value_c0(char *param_1,undefined4 param_2)

{
  *(undefined4 *)(*(char **)(g_ptank_plugin_offset + *(char **)(param_1 + 4)) + 0xc0) = param_2;
  *(uint *)(*(char **)(g_ptank_plugin_offset + *(char **)(param_1 + 4)) + 0x40) |= 0x20000;
  return;
}
