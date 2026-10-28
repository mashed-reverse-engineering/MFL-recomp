// cluster-entries: weapon_flashbang_start_effect
/* Installed cluster weapon_flashbang_start_effect (tools/cluster_install.py from output/cluster_pilot11/weapon_flashbang_start_effect.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*00437f50*/ extern int screen_shake_set();
/*006a5d18*/ extern undefined g_weapon_6a5d18;
/*006a5d78*/ extern char g_weapon_6a5d78[];
/* weapon_flashbang_start_effect */

/* "state 2, finds a free flash slot, stores the pickup position and sets the screen shake to 10" */

/* Sets weapon state to 2, takes the first free flash slot (slot[3]==0), stores pickup position (+0x30..0x38) in it and starts screen shake 10. */
void weapon_flashbang_start_effect_n(int state)

{
  char *pickup_obj;
  undefined4 *flash_slot;
  
  *(undefined4 *)(state + 0xc) = 2;
  flash_slot = (undefined4 *)&g_weapon_6a5d18;
  do {
    if (flash_slot[3] == 0) {
      flash_slot[4] = 0;
      flash_slot[5] = 0;
      flash_slot[3] = 1;
      pickup_obj = *(char **)(state + 8);
      *flash_slot = *(undefined4 *)(pickup_obj + 0x30);
      flash_slot[1] = *(undefined4 *)(pickup_obj + 0x34);
      flash_slot[2] = *(undefined4 *)(pickup_obj + 0x38);
      screen_shake_set(10);
      return;
    }
    flash_slot = flash_slot + 6;
  } while ((int)flash_slot < ((int)&g_weapon_6a5d78));
  screen_shake_set(10);
  return;
}

/* entry thunk at the original address of weapon_flashbang_start_effect: register inputs (edx) -> natural C weapon_flashbang_start_effect_n; ret 0 */
__declspec(naked) void weapon_flashbang_start_effect(void)
{
    __asm {
        push edx
        call weapon_flashbang_start_effect_n
        add esp, 4
        ret
    }
}
