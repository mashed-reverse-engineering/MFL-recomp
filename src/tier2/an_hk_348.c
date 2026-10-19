#include "mfl_tier2.h"
/*006592c4*/ extern undefined g_load_6592c4;
/*006592dc*/ extern undefined4 g_hudpanel_6592dc;
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

/* hudpanel_render_if_loaded
 * Draws the HUD panel group (hudpanel_render_group) only when the HUD panel
 * flag g_hudpanel_6592dc is non-zero; otherwise does nothing.
 * Takes no parameters, returns nothing. The group is called with
 * &g_load_6592c4 passed in eax (via hudpanel_render_group_r).
 */

/* "hudpanel_render_group when g_hudpanel_6592dc is set" */

void hudpanel_render_if_loaded(void)

{
  if (g_hudpanel_6592dc != 0) {
    hudpanel_render_group_r(&g_load_6592c4);
    return;
  }
  return;
}


