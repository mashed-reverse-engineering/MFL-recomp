// cluster-entries: weapon_flashbang_fire
/* Installed cluster weapon_flashbang_fire (tools/cluster_install.py from output/cluster_pilot_kb/weapon_flashbang_fire.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*004726a0*/ extern int sfx_play_at();
/* weapon_flashbang_fire */

static int weapon_flashbang_start_effect_r(int p0);
extern void weapon_flashbang_start_effect(void);
/* On event 2 with state stage 1: starts the flash effect and plays sfx 0x18 at the muzzle position. */
void weapon_flashbang_fire(WeaponSlot *slot, int event)

{
  struct {
    float pos_x;
    float pos_y;
    float pos_z;
  } frame;
  
  if ((event == 2) && (*(int *)(slot->state + 0xc) == 1)) {
    weapon_flashbang_start_effect_r(slot->state);
    frame.pos_x = slot->muzzle_position[0];
    frame.pos_y = slot->muzzle_position[1];
    frame.pos_z = slot->muzzle_position[2];
    sfx_play_at(0x18,&frame.pos_x);
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
