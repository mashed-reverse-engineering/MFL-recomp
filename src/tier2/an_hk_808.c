#include "mfl_tier2.h"
/*004726a0*/ extern int sfx_play_at();
extern void weapon_mine_drop_n(int mine_pool);

#define FIRE_EVENT_MINE_DROP (2) /* (inferred) */
#define SFX_MINE_DROP (0x1b) /* (inferred) */

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
    weapon_mine_drop_n((int)((void *)slot->state));
    sfx_play_at(SFX_MINE_DROP, (*(int *)(slot->state + 0xc) + 0x30));
    return;
  }
  return;
}


