// cluster-entries: weapon_flashbang_fire
/* Installed cluster weapon_flashbang_fire (tools/cluster_install.py from output/cluster_pilot_kb/weapon_flashbang_fire.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*004726a0*/ extern int sfx_play_at();
/* weapon_flashbang_fire */

static int weapon_flashbang_start_effect_r(int p0);
extern void weapon_flashbang_start_effect(void);
void weapon_flashbang_fire(WeaponSlot *param_1, int param_2)

{
  struct {
    float fStack_c;
    float fStack_8;
    float fStack_4;
  } frame;
  
  if ((param_2 == 2) && (*(int *)(param_1->state + 0xc) == 1)) {
    weapon_flashbang_start_effect_r(param_1->state);
    frame.fStack_c = param_1->muzzle_position[0];
    frame.fStack_8 = param_1->muzzle_position[1];
    frame.fStack_4 = param_1->muzzle_position[2];
    sfx_play_at(0x18,&frame.fStack_c);
  }
  return;
}

/* weapon_flashbang_start_effect_r: C -> original weapon_flashbang_start_effect (registers edx) */
__declspec(naked) int weapon_flashbang_start_effect_r(int p0)
{
    __asm {
        mov edx, dword ptr [esp + 4]
        call weapon_flashbang_start_effect
        ret
    }
}
