#include "mfl_tier2.h"
/*006a29b8*/ extern undefined4 g_weapon_6a29b8;
/*006a2a28*/ extern undefined g_weapon_6a2a28;
static int weapon_pickup_slot_clear_a_r(int reg_esi);
extern void weapon_pickup_slot_clear_a(void);
__declspec(naked) int weapon_pickup_slot_clear_a_r(int reg_esi)
{
    __asm {
        push esi
        mov esi, dword ptr [esp + 8]
        call weapon_pickup_slot_clear_a
        pop esi
        ret
    }
}

static int weapon_pickup_slot_clear_b_r(int reg_esi);
extern void weapon_pickup_slot_clear_b(void);
extern char g_explosion_queue[];
__declspec(naked) int weapon_pickup_slot_clear_b_r(int reg_esi)
{
    __asm {
        push esi
        mov esi, dword ptr [esp + 8]
        call weapon_pickup_slot_clear_b
        pop esi
        ret
    }
}

#define PICKUP_SLOT_B_STRIDE (0x110)

/* weapon_pickups_clear */

/* Clears all weapon pickup slots (inferred: called when pickups are reset).
   Takes no parameters, returns nothing. Walks two arrays in order:
   1. weapon_pickup_slot_clear_a on each 0x1C-byte slot in g_weapon_6a29b8..g_weapon_6a2a28,
   2. weapon_pickup_slot_clear_b on each PICKUP_SLOT_B_STRIDE-byte entry in g_weapon_6a2a28..g_explosion_queue. */
/* "calls course_fn_45e3d0 for every 0x1C byte record from 0x6A29B8 to 0x6A2A28" */

void weapon_pickups_clear(void)

{
  undefined4 *pickup_slot;
  undefined *pickup_slot_b;

  /* first array: 0x1C-byte slots (7 dwords each) */
  pickup_slot = &g_weapon_6a29b8;
  do {
    weapon_pickup_slot_clear_a_r(pickup_slot);
    pickup_slot = pickup_slot + 7;
  } while ((LONG_PTR)pickup_slot < ((LONG_PTR)&g_weapon_6a2a28));
  /* second array: strided entries up to the explosion queue */
  pickup_slot_b = &g_weapon_6a2a28;
  do {
    weapon_pickup_slot_clear_b_r(pickup_slot_b);
    pickup_slot_b = pickup_slot_b + PICKUP_SLOT_B_STRIDE;
  } while ((LONG_PTR)pickup_slot_b < ((LONG_PTR)&g_explosion_queue));
  return;
}


