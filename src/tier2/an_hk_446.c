#include "mfl_tier2.h"
/*006a6238*/ extern undefined4 g_weapon_6a6238;
/*006a7c78*/ extern undefined g_weapon_6a7c78;
/*006a7d28*/ extern undefined1 g_weapon_6a7d28;
static int weapon_effect_slot_clear_6a7c78_r(undefined4 * reg_eax);
extern void weapon_effect_slot_clear_6a7c78(void);
__declspec(naked) int weapon_effect_slot_clear_6a7c78_r(undefined4 * reg_eax)
{
    __asm {
        mov eax, dword ptr [esp + 4]
        call weapon_effect_slot_clear_6a7c78
        ret
    }
}

static int weapon_fx_slot_clear_c_r(undefined4 * reg_eax);
extern void weapon_fx_slot_clear_c(void);
extern char g_weapon_6a7c38[];
extern char g_weapon_6a7cb8[];
#define FX_RECORD_SIZE (0x10)
#define WEAPON_SLOT_DWORDS (0x1a)
__declspec(naked) int weapon_fx_slot_clear_c_r(undefined4 * reg_eax)
{
    __asm {
        mov eax, dword ptr [esp + 4]
        call weapon_fx_slot_clear_c
        ret
    }
}

/*
 * Resets the weapon effect state: clears the 4 FX records at 0x6A7C78
 * (FX_RECORD_SIZE bytes apart), then clears every weapon effect slot at 0x6A6238
 * (WEAPON_SLOT_DWORDS dwords apart) and its active bit in the bitmap at 0x6A7D28.
 * Takes no parameters and returns nothing.
 * (inferred: the slot count is the number of slots between 0x6A6238 and 0x6A7C38)
 */
void weapon_effects_clear_6a7c78(void)

{
  byte bit_index;
  int bitmap_byte_index;
  undefined *record_ptr;
  int slot_index;
  undefined4 *slot_ptr;
  
  /* clear each FX record in turn, up to the end of the record block at 0x6A7CB8 */
  record_ptr = &g_weapon_6a7c78;
  do {
    weapon_fx_slot_clear_c_r(record_ptr);
    record_ptr = record_ptr + FX_RECORD_SIZE;
  } while ((LONG_PTR)record_ptr < ((LONG_PTR)&g_weapon_6a7cb8));
  /*
   * For each slot i: clear the slot, then clear bit (i & 7) of bitmap byte (i >> 3)
   * at 0x6A7D28 so the slot is no longer marked active.
   */
  slot_index = 0;
  slot_ptr = &g_weapon_6a6238;
  do {
    weapon_effect_slot_clear_6a7c78_r(slot_ptr);
    bitmap_byte_index = slot_index >> 3;
    bit_index = (byte)slot_index;
    slot_ptr = slot_ptr + WEAPON_SLOT_DWORDS;
    slot_index = slot_index + 1;
    (&g_weapon_6a7d28)[bitmap_byte_index] = (&g_weapon_6a7d28)[bitmap_byte_index] & ~('\x01' << (bit_index & 7));
  } while ((LONG_PTR)slot_ptr < ((LONG_PTR)&g_weapon_6a7c38));
  return;
}


