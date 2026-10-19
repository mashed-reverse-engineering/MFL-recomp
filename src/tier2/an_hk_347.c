#include "mfl_tier2.h"
/*00658b30*/ extern undefined g_load_658b30;
/*00658b48*/ extern undefined4 g_hudpanel_658b48;
static int hudpanel_render_group_r(int * reg_eax);
extern void hudpanel_render_group(void);
__declspec(naked) int hudpanel_render_group_r(int * reg_eax)
{
    __asm {
        mov eax, dword ptr [esp + 4]
        call hudpanel_render_group
        ret
    }
}

/* hudpanel_render_if_loaded_b */

/* "hudpanel_render_group when g_hudpanel_658b48 is set" */

/* Draws the HUD panel group if g_hudpanel_658b48 is nonzero; does nothing otherwise.
 * Takes no parameters and returns nothing. The group is passed &g_load_658b30 in eax
 * (inferred: the load-state block for the panel). */

void hudpanel_render_if_loaded_b(void)

{
  if (g_hudpanel_658b48 != 0) {
    hudpanel_render_group_r(&g_load_658b30);
    return;
  }
  return;
}


