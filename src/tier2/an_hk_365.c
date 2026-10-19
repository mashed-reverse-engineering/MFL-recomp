#include "mfl_tier2.h"
/*004f6f40*/ extern RwBool RpClumpDestroy(struct RpClump *p0);
/*00559bb0*/ extern void FUN_00559bb0(undefined4 p0);
/*006c51b0*/ extern undefined g_gfx_6c51b0;
/*006c57f8*/ extern undefined g_gfx_6c57f8;
/*006c5df8*/ extern undefined4 g_gfx_6c5df8;
/*006c5dfc*/ extern undefined4 g_load_6c5dfc;
/*006c5e08*/ extern undefined4 g_load_6c5e08;
/*006c5e0c*/ extern undefined4 g_load_6c5e0c;
static int sprite_slot_destroy_atomic_44_r(int reg_esi);
extern void sprite_slot_destroy_atomic_44(void);
extern char g_gfx_6c57b0[];
#define SPRITE_SLOT_STRIDE (0x60)
__declspec(naked) int sprite_slot_destroy_atomic_44_r(int reg_esi)
{
    __asm {
        push esi
        mov esi, dword ptr [esp + 8]
        call sprite_slot_destroy_atomic_44
        pop esi
        ret
    }
}

/* sprite_slots_destroy */

/* "destroys the atomic of every sprite slot" */

void sprite_slots_destroy(void)

{
  undefined *slot;

  /* first slot array: from g_gfx_6c57f8 up to g_gfx_6c5df8 */
  slot = &g_gfx_6c57f8;
  do {
    sprite_slot_destroy_atomic_44_r(slot);
    slot = slot + SPRITE_SLOT_STRIDE;
  } while ((LONG_PTR)slot < ((LONG_PTR)&g_gfx_6c5df8));
  /* second slot array: from g_gfx_6c51b0 up to g_gfx_6c57b0 (inferred) */
  slot = &g_gfx_6c51b0;
  do {
    sprite_slot_destroy_atomic_44_r(slot);
    slot = slot + SPRITE_SLOT_STRIDE;
  } while ((LONG_PTR)slot < ((LONG_PTR)&g_gfx_6c57b0));
  /* then destroy the two clumps, each preceded by a FUN_00559bb0 call on its load id (inferred) */
  FUN_00559bb0(g_load_6c5e08);
  RpClumpDestroy(g_gfx_6c5df8);
  FUN_00559bb0(g_load_6c5e0c);
  RpClumpDestroy(g_load_6c5dfc);
  return;
}


