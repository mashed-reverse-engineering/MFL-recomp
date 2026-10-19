#include "mfl_tier2.h"
/*0065a9e8*/ extern undefined4 car_damage;
static int debris_slot_clear_r(int reg_esi);
extern void debris_slot_clear(void);

/* car_damage holds 10 undefined4 damage slots per car (stride inferred from this function) */
#define CAR_DAMAGE_STRIDE (10)

__declspec(naked) int debris_slot_clear_r(int reg_esi)
{
    __asm {
        push esi
        mov esi, dword ptr [esp + 8]
        call debris_slot_clear
        pop esi
        ret
    }
}

/*
 * debris_slot_clear_forward: clears the debris damage slot of one car.
 * car_index: index of the car (inferred from the car_damage stride).
 * Passes the address of that car's damage block to debris_slot_clear. No return value.
 */

/* "calls debris_slot_clear" */

void debris_slot_clear_forward(int car_index)

{
  /* pointer to the car's damage block, passed to debris_slot_clear */
  debris_slot_clear_r(&car_damage + car_index * CAR_DAMAGE_STRIDE);
  return;
}


