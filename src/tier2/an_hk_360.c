#include "mfl_tier2.h"
/*00463cc0*/ extern int weapon_mine_update_one();
/*00486c40*/ extern int particles_flush();
/*004d15a0*/ extern struct RwFrame * RwFrameTransform(struct RwFrame *p0, RwMatrix *p1, enum RwOpCombineType p2);
/*006a7d38*/ extern undefined g_weapon_6a7d38;
/*006a8ab8*/ extern undefined g_weapon_6a8ab8;
/*006a8b08*/ extern undefined4 g_weapon_mines_6a8b08;
static int weapon_fx_pool_clear_b_r(int reg_edi);
extern void weapon_fx_pool_clear_b(void);
extern char g_weapon_6a8b68[];
#define MINE_SLOT_WORDS (6)
#define MINE_RECORD_SIZE (0x6c)
__declspec(naked) int weapon_fx_pool_clear_b_r(int reg_edi)
{
    __asm {
        push edi
        mov edi, dword ptr [esp + 8]
        call weapon_fx_pool_clear_b
        pop edi
        ret
    }
}

/* Per-frame update of the weapon mines (inferred from the callees).
 * Takes no parameters and returns nothing.
 * 1. Walks the mine slot table from g_weapon_mines_6a8b08 up to g_weapon_6a8b68,
 *    MINE_SLOT_WORDS ints per slot, and handles every slot with a non-zero state.
 * 2. Walks the mine records from g_weapon_6a7d38 up to g_weapon_6a8ab8,
 *    MINE_RECORD_SIZE bytes per record, and updates each one.
 * 3. Flushes the particles from g_weapon_6a8ab8. */
void weapon_mines_update(void)

{
  int slot_state;
  int *mine_slot;
  undefined *mine;

  /* Pass 1: mine slot table. */
  mine_slot = &g_weapon_mines_6a8b08;
  do {
    slot_state = *mine_slot;
    if (slot_state != 0) {
      /* State 1: re-transform the slot's frame with replace combine (inferred). */
      if (slot_state == 1) {
        RwFrameTransform(*(RwFrame **)(mine_slot[1] + 4),(RwMatrix *)mine_slot[-1],rwCOMBINEREPLACE);
      }
      /* State 2: clear the FX pool (inferred). */
      else if (slot_state == 2) {
        weapon_fx_pool_clear_b_r(mine_slot + -4);
      }
    }
    mine_slot = mine_slot + MINE_SLOT_WORDS;
  } while ((LONG_PTR)mine_slot < ((LONG_PTR)&g_weapon_6a8b68));
  /* Pass 2: mine records, then flush particles. */
  mine = &g_weapon_6a7d38;
  do {
    weapon_mine_update_one(mine);
    mine = mine + MINE_RECORD_SIZE;
  } while ((LONG_PTR)mine < ((LONG_PTR)&g_weapon_6a8ab8));
  particles_flush(&g_weapon_6a8ab8);
  return;
}


