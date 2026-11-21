#include "mfl_tier2.h"
/*006a7d38*/ extern undefined g_weapon_6a7d38;
/*006a8af8*/ extern undefined g_weapon_6a8af8;
extern void weapon_fx_pool_clear_b_n(int);

extern void weapon_fx_slot_clear_b_n(undefined4 *fx_slot);
extern char g_weapon_6a8ab8[];
extern char g_weapon_6a8b58[];
#define WEAPON_FX_POOL_RECORD_SIZE (0x18)
#define WEAPON_FX_SLOT_STRIDE (0x6c)

/* weapon_effects_clear_6a8af8 */

/* "clears every 0x18 dword record from 0x6A8AF8 to 0x6A8B58" */

/*
 * Resets all weapon effect (FX) storage in one pass: the pool of 0x18-byte
 * records at g_weapon_6a8af8 .. g_weapon_6a8b58, then the 0x6c-byte slots at
 * g_weapon_6a7d38 .. g_weapon_6a8ab8 (inferred: slot record layout).
 * Takes no parameters and returns nothing.
 */
void weapon_effects_clear_6a8af8(void)

{
  undefined *cursor;

  /* walk the record pool from its start to its end, clearing each record */
  cursor = &g_weapon_6a8af8;
  do {
    weapon_fx_pool_clear_b_n(cursor);
    cursor = cursor + WEAPON_FX_POOL_RECORD_SIZE;
  } while ((LONG_PTR)cursor < ((LONG_PTR)&g_weapon_6a8b58));
  /* then walk the slot table the same way, clearing each slot (inferred) */
  cursor = &g_weapon_6a7d38;
  do {
    weapon_fx_slot_clear_b_n(cursor);
    cursor = cursor + WEAPON_FX_SLOT_STRIDE;
  } while ((LONG_PTR)cursor < ((LONG_PTR)&g_weapon_6a8ab8));
  return;
}


