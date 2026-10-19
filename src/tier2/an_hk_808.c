#include "mfl_tier2.h"
/*004726a0*/ extern int sfx_play_at();
static int weapon_mine_drop_r(void *reg_eax);
extern void weapon_mine_drop(void);

#define FIRE_EVENT_MINE_DROP (2) /* (inferred) */
#define SFX_MINE_DROP (0x1b) /* (inferred) */
__declspec(naked) int weapon_mine_drop_r(void *reg_eax)
{
    __asm {
        mov eax, dword ptr [esp + 4]
        call weapon_mine_drop
        ret
    }
}

/* weapon_mines_fire
 * Fire handler for the mine weapon slot (inferred from the body).
 * slot: the weapon slot being fired; its state pointer is the per player mine state.
 * fire_event: fire event code (inferred); FIRE_EVENT_MINE_DROP means drop a mine.
 * Drops a mine from the slot's state and plays the drop sound (SFX_MINE_DROP) with
 * an int computed from state+0xc, plus 0x30 (meaning inferred). Returns nothing.
 */

void weapon_mines_fire(WeaponSlot *slot,int fire_event)

{
  if (fire_event == FIRE_EVENT_MINE_DROP) {
    weapon_mine_drop_r((void *)slot->state);
    sfx_play_at(SFX_MINE_DROP, (*(int *)(slot->state + 0xc) + 0x30));
    return;
  }
  return;
}


