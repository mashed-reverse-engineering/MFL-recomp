#include "mfl_tier2.h"
/*00659ac0*/ extern undefined4 g_hudpanel_659ac0;
/*00659b08*/ extern undefined4 g_stopclock_659b08;
static int stopclock_render_parts_by_team_r(int * reg_esi, int param_2);
extern void stopclock_render_parts_by_team(void);
__declspec(naked) int stopclock_render_parts_by_team_r(int * reg_esi, int param_2)
{
    __asm {
        push esi
        push dword ptr [esp + 12]
        mov esi, dword ptr [esp + 12]
        call stopclock_render_parts_by_team
        add esp, 4
        pop esi
        ret
    }
}

/* stopclock_render */

/* "renders the stop clock clump when 0x659B08 is set" */

/* Draws the stop clock HUD parts (in g_hudpanel_659ac0) if g_stopclock_659b08 is non-zero.
 * render_arg (inferred): render argument, forwarded unchanged to stopclock_render_parts_by_team_r.
 * Returns nothing.
 */
void __cdecl stopclock_render(int render_arg)

{
  /* Only draw when the stop clock is enabled. */
  if (g_stopclock_659b08 != 0) {
    stopclock_render_parts_by_team_r(&g_hudpanel_659ac0,render_arg);
  }
  return;
}


