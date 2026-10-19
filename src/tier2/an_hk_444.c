#include "mfl_tier2.h"
/*006a5dd0*/ extern undefined g_weapon_6a5dd0;
/*006a5ff0*/ extern undefined g_rocket_state;
static int weapon_effect_slot_reset_6a5dd0_r(undefined4 * reg_esi);
extern void weapon_effect_slot_reset_6a5dd0(void);
__declspec(naked) int weapon_effect_slot_reset_6a5dd0_r(undefined4 * reg_esi)
{
    __asm {
        push esi
        mov esi, dword ptr [esp + 8]
        call weapon_effect_slot_reset_6a5dd0
        pop esi
        ret
    }
}

static int weapon_fx_clump_remove_b_r(int reg_esi);
extern void weapon_fx_clump_remove_b(void);
extern char g_weapon_effects_6a5fec[];
#define WEAPON_RECORD_SIZE (0x6c)
#define ROCKET_STATE_SIZE (0x2c)
__declspec(naked) int weapon_fx_clump_remove_b_r(int reg_esi)
{
    __asm {
        push esi
        mov esi, dword ptr [esp + 8]
        call weapon_fx_clump_remove_b
        pop esi
        ret
    }
}

/* weapon_effects_clear_6a5dd0 */

/* Initialises and resets every weapon effect record at g_weapon_6a5dd0 (WEAPON_RECORD_SIZE bytes each),
   up to g_weapon_effects_6a5fec. Each weapon record is paired with one rocket state slot
   (ROCKET_STATE_SIZE bytes, from g_rocket_state) (inferred). Takes no parameters, returns nothing. */

void weapon_effects_clear_6a5dd0(void)

{
  undefined *rocket_slot;
  undefined *weapon_slot;

  rocket_slot = &g_rocket_state;
  weapon_slot = &g_weapon_6a5dd0;
  /* Walk both arrays in step: reset the rocket slot, clear the weapon record, advance both. */
  do {
    weapon_effect_slot_reset_6a5dd0_r(rocket_slot);
    weapon_fx_clump_remove_b_r(weapon_slot);
    weapon_slot = weapon_slot + WEAPON_RECORD_SIZE;
    rocket_slot = rocket_slot + ROCKET_STATE_SIZE;
  } while ((LONG_PTR)weapon_slot < ((LONG_PTR)&g_weapon_effects_6a5fec));
  return;
}


