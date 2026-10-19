#include "mfl_tier2.h"
/*004726a0*/ extern int sfx_play_at();
static int weapon_drum_drop_r(void *reg_edi);
extern void weapon_drum_drop(void);
/* action passed to weapon_drum_fire (inferred: fire event) */
#define WEAPON_ACTION_FIRE (2)
/* sound id played by sfx_play_at when a drum fires (inferred) */
#define SFX_DRUM_FIRE (0x12)
__declspec(naked) int weapon_drum_drop_r(void *reg_edi)
{
    __asm {
        push edi
        mov edi, dword ptr [esp + 8]
        call weapon_drum_drop
        pop edi
        ret
    }
}

/* Handles an action on a drum weapon slot. On WEAPON_ACTION_FIRE it drops the drum
   (weapon_drum_drop, via weapon_drum_drop_r with the slot's state as edi) and plays
   SFX_DRUM_FIRE at the slot's muzzle position (inferred from callee names).
   weapon_slot: the weapon slot firing. action: WEAPON_ACTION_FIRE or another action code (ignored). Returns nothing. */

void weapon_drum_fire(WeaponSlot *weapon_slot,int action)

{
  struct {
    float fStack_c;
    float fStack_8;
    float fStack_4;
  } frame;
  
  /* fire: drop the drum, then copy the muzzle position into a scratch vector and play the sound there */
  if (action == WEAPON_ACTION_FIRE) {
    weapon_drum_drop_r((void *)weapon_slot->state);
    frame.fStack_c = weapon_slot->muzzle_position[0];
    frame.fStack_8 = weapon_slot->muzzle_position[1];
    frame.fStack_4 = weapon_slot->muzzle_position[2];
    sfx_play_at(SFX_DRUM_FIRE,&frame.fStack_c);
  }
  return;
}


