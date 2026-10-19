#include "mfl_tier2.h"
/*006a5a40*/ extern undefined g_weapon_6a5a40;
static int weapon_fx_explosion_slot_clear_r(undefined4 * reg_esi);
extern void weapon_fx_explosion_slot_clear(void);
extern char g_weapon_6a5c60[];

/* Explosion slot table: entries are ENTRY_STRIDE bytes apart, state at ENTRY_STATE_OFFSET (inferred) */
#define ENTRY_STRIDE (0x44)
#define ENTRY_STATE_OFFSET (0x2c)
/* slot state must be strictly below this to be cleared (inferred) */
#define EXPLOSION_STATE_LIMIT (6)

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

/* Walks the explosion slot table from g_weapon_6a5a40 up to g_weapon_6a5c60 and clears
   every slot whose state is in the range (1, EXPLOSION_STATE_LIMIT) exclusive (inferred).
   Takes no parameters, returns nothing. */

void weapon_drum_hook_2c(void)

{
  undefined *entry;

  entry = &g_weapon_6a5a40;
  /* one pass over every slot in the table, ENTRY_STRIDE bytes at a time */
  do {
    /* clear the slot only when its state is inside the range (inferred) */
    if ((1 < *(int *)(entry + ENTRY_STATE_OFFSET)) && (*(int *)(entry + ENTRY_STATE_OFFSET) < EXPLOSION_STATE_LIMIT)) {
      weapon_fx_explosion_slot_clear_r(entry);
    }
    entry = entry + ENTRY_STRIDE;
  } while ((LONG_PTR)entry < ((LONG_PTR)&g_weapon_6a5c60));
  return;
}


