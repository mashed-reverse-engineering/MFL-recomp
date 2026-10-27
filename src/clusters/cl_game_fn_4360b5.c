// cluster-entries: game_fn_4360b5
/* Installed cluster game_fn_4360b5 (tools/cluster_install.py from output/cluster_pilot_md/game_fn_4360b5.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*007eebc0*/ extern struct GameState g_game_state;
/* game_fn_4360b5 */

/* "event progress word of a career slot: g_game_state.progress[slot * 2]; the slot arrives in eax" */
unsigned int game_fn_4360b5_n(int in_eax)
{
  return g_game_state.progress[in_eax * 2];
}

/* entry thunk at the original address of game_fn_4360b5: register inputs (eax) -> natural C game_fn_4360b5_n; ret 0 */
__declspec(naked) void game_fn_4360b5(void)
{
    __asm {
        push ecx
        push edx
        push eax
        call game_fn_4360b5_n
        add esp, 4
        pop edx
        pop ecx
        ret
    }
}
