#include "mfl_tier2.h"
/*004c66a0*/ extern void * mem_zero(void * p0, int p1);
/*006c51a8*/ extern undefined4 g_gfx_6c51a8;
/*006c51b0*/ extern undefined g_gfx_6c51b0;
/*006c57b0*/ extern undefined4 g_gfx_6c57b0;
/*006c57f8*/ extern undefined g_gfx_6c57f8;
static int effect_sprite_reset_r(undefined4 * reg_eax);
extern void effect_sprite_reset(void);
extern char g_gfx_6c5df8[];

/* size in bytes of one sprite slot */
#define SPRITE_SLOT_SIZE (0x60)

__declspec(naked) int effect_sprite_reset_r(undefined4 * reg_eax)
{
    __asm {
        mov eax, dword ptr [esp + 4]
        call effect_sprite_reset
        ret
    }
}

/* sprite_slots_clear
 * Resets every effect sprite slot: walks the slot table at 0x6C57F8 up to
 * 0x6C5DF8 and the second table at 0x6C51B0 up to 0x6C57B0, calling
 * effect_sprite_reset on each slot. Then zeroes the 4-byte counters at
 * 0x6C57B0 and 0x6C51A8 (inferred: counters for the two tables).
 * Parameters: none. Returns nothing.
 */

void sprite_slots_clear(void)

{
  undefined *slot;

  /* first table: 0x6C57F8 .. 0x6C5DF8, one slot per step (inferred) */
  slot = &g_gfx_6c57f8;
  do {
    effect_sprite_reset_r(slot);
    slot = slot + SPRITE_SLOT_SIZE;
  } while ((LONG_PTR)slot < ((LONG_PTR)&g_gfx_6c5df8));
  /* second table: 0x6C51B0 .. 0x6C57B0, one slot per step (inferred) */
  slot = &g_gfx_6c51b0;
  do {
    effect_sprite_reset_r(slot);
    slot = slot + SPRITE_SLOT_SIZE;
  } while ((LONG_PTR)slot < ((LONG_PTR)&g_gfx_6c57b0));
  mem_zero(&g_gfx_6c57b0,4);
  mem_zero(&g_gfx_6c51a8,4);
  return;
}


