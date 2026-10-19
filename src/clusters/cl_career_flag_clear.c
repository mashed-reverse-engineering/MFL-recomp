// cluster-entries: career_flag_clear
/* Installed cluster career_flag_clear (tools/cluster_install.py from output/cluster_pilot_hb1/career_flag_clear.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*007eebc0*/ extern struct GameState g_game_state;
/* career_flag_clear */

/* clears one entry of g_enabled_flags */

undefined4 career_flag_clear_n(undefined4 param_1, int in_eax)

{
  
  switch(param_1) {
  case 0:
    g_game_state.enabled[in_eax * 0x12] = 0;
    return 0;
  case 1:
    g_game_state.enabled[in_eax * 0x12 + 1] = 0;
    return 0;
  case 2:
    g_game_state.enabled[in_eax * 0x12 + 2] = 0;
    return 0;
  case 3:
    g_game_state.enabled[in_eax * 0x12 + 3] = 0;
    return 0;
  case 4:
    g_game_state.enabled[in_eax * 0x12 + 4] = 0;
    return 0;
  case 5:
    g_game_state.enabled[in_eax * 0x12 + 5] = 0;
    return 0;
  case 6:
    g_game_state.enabled[in_eax * 0x12 + 6] = 0;
    return 0;
  case 7:
    g_game_state.enabled[in_eax * 0x12 + 7] = 0;
    return 0;
  case 8:
    g_game_state.enabled[in_eax * 0x12 + 8] = 0;
    return 0;
  case 0xb:
    g_game_state.enabled[in_eax * 0x12 + 0xb] = 0;
    return 0;
  case 0xc:
    g_game_state.enabled[in_eax * 0x12 + 0xc] = 0;
    return 0;
  case 0xd:
    g_game_state.enabled[in_eax * 0x12 + 0xd] = 0;
    return 0;
  case 0xe:
    g_game_state.enabled[in_eax * 0x12 + 0xe] = 0;
    return 0;
  case 0xf:
    g_game_state.enabled[in_eax * 0x12 + 0xf] = 0;
    return 0;
  case 0x10:
    g_game_state.enabled[in_eax * 0x12 + 0x10] = 0;
    return 0;
  case 0x11:
    g_game_state.enabled[in_eax * 0x12 + 0x11] = 0;
  }
  return 0;
}

/* entry thunk at the original address of career_flag_clear: register inputs (ecx,eax) -> natural C career_flag_clear_n; ret 0 */
__declspec(naked) void career_flag_clear(void)
{
    __asm {
        push eax
        push ecx
        call career_flag_clear_n
        add esp, 8
        ret
    }
}
