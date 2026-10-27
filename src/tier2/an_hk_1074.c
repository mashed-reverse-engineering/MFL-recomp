#include "mfl_tier2.h"
/* atomic_set_material_alpha_value_cb_484990 */

/* function pointer pushed/stored by atomic_set_material_alpha_value (MOV dword ptr [ESP +
   0xc],0x484990) */

char *atomic_set_material_alpha_value_cb_484990(char *param_1,undefined1 *param_2)

{
  *(undefined1 *)(param_1 + 7) = *param_2;
  *(undefined4 *)(param_1 + 4) = *(undefined4 *)(param_1 + 4);
  return param_1;
}


