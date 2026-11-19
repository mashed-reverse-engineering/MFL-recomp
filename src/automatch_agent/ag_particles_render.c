#include "mfl_tier2.h"
/*007da6bc*/ extern undefined4 g_ptank_plugin_offset;


/* particles_render */

/*  */

void particles_render(char *param_1)

{
  if (*(int *)(*(char **)(*(char **)(param_1 + 4) + g_ptank_plugin_offset) + 4) != 0) {
                    /* WARNING: Could not recover jumptable at 0x00486f0b. Too many branches */
                    /* WARNING: Treating indirect jump as call */
    int p = *(int *)(param_1 + 4);
    (*(void (__cdecl **)(int))(p + 0x48))(p);
    return;
  }
  return;
}
