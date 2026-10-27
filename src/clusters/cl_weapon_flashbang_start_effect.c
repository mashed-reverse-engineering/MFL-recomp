// cluster-entries: weapon_flashbang_start_effect
/* Installed cluster weapon_flashbang_start_effect (tools/cluster_install.py from output/cluster_pilot11/weapon_flashbang_start_effect.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*00437f50*/ extern int screen_shake_set();
/*006a5d18*/ extern undefined g_weapon_6a5d18;
/*006a5d78*/ extern char g_weapon_6a5d78[];
/* weapon_flashbang_start_effect */

/* "state 2, finds a free flash slot, stores the pickup position and sets the screen shake to 10" */

void weapon_flashbang_start_effect_n(int in_edx)

{
  char *iVar1;
  undefined4 *puVar2;
  
  *(undefined4 *)(in_edx + 0xc) = 2;
  puVar2 = (undefined4 *)&g_weapon_6a5d18;
  do {
    if (puVar2[3] == 0) {
      puVar2[4] = 0;
      puVar2[5] = 0;
      puVar2[3] = 1;
      iVar1 = *(char **)(in_edx + 8);
      *puVar2 = *(undefined4 *)(iVar1 + 0x30);
      puVar2[1] = *(undefined4 *)(iVar1 + 0x34);
      puVar2[2] = *(undefined4 *)(iVar1 + 0x38);
      screen_shake_set(10);
      return;
    }
    puVar2 = puVar2 + 6;
  } while ((int)puVar2 < ((int)&g_weapon_6a5d78));
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
