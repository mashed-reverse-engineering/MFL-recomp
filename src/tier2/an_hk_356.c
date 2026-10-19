#include "mfl_tier2.h"
/*006a5a40*/ extern undefined g_weapon_6a5a40;
/*006a5c60*/ extern undefined g_weapon_6a5c60;
static int weapon_fx_explosion_pool_clear_r(int reg_edi);
extern void weapon_fx_explosion_pool_clear(void);
__declspec(naked) int weapon_fx_explosion_pool_clear_r(int reg_edi)
{
    __asm {
        push edi
        mov edi, dword ptr [esp + 8]
        call weapon_fx_explosion_pool_clear
        pop edi
        ret
    }
}

static int weapon_fx_explosion_slot_clear_r(undefined4 * reg_esi);
extern void weapon_fx_explosion_slot_clear(void);
extern char g_weapon_6a5d10[];
#define EFFECT_RECORD_SIZE (0x2c)
#define SLOT_RECORD_SIZE (0x44) /* inferred */
__declspec(naked) int weapon_fx_explosion_slot_clear_r(undefined4 * reg_esi)
{
    __asm {
        push esi
        mov esi, dword ptr [esp + 8]
        call weapon_fx_explosion_slot_clear
        pop esi
        ret
    }
}

/* weapon_effects_clear_6a5c60 */

/* "clears every 0x2C byte effect record from 0x6A5C60 to 0x6A5D10" */

/* Clears the explosion weapon effect state: every effect record in
   g_weapon_6a5c60..g_weapon_6a5d10, then every slot in g_weapon_6a5a40..g_weapon_6a5c60
   (slot stride inferred). Takes no parameters, returns nothing. */
void weapon_effects_clear_6a5c60(void)

{
  undefined *record;

  /* pass 1: walk the effect records, clearing each explosion pool entry */
  record = &g_weapon_6a5c60;
  do {
    weapon_fx_explosion_pool_clear_r(record);
    record = record + EFFECT_RECORD_SIZE;
  } while ((LONG_PTR)record < ((LONG_PTR)&g_weapon_6a5d10));
  /* pass 2: walk the slots below the effect records, clearing each slot (inferred) */
  record = &g_weapon_6a5a40;
  do {
    weapon_fx_explosion_slot_clear_r(record);
    record = record + SLOT_RECORD_SIZE;
  } while ((LONG_PTR)record < ((LONG_PTR)&g_weapon_6a5c60));
  return;
}


