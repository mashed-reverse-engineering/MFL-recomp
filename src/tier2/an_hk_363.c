#include "mfl_tier2.h"
/*006c51a8*/ extern undefined4 g_gfx_6c51a8;
/*006c51b0*/ extern undefined g_gfx_6c51b0;
/*006c57b0*/ extern undefined4 g_gfx_6c57b0;
/*006c57f8*/ extern undefined g_gfx_6c57f8;
static int effect_sprite_update_r(int reg_esi);
extern void effect_sprite_update(void);
extern char g_gfx_6c5df8[];
#define EFFECT_SPRITE_STRIDE (0x60)
__declspec(naked) int effect_sprite_update_r(int reg_esi)
{
    __asm {
        push esi
        mov esi, dword ptr [esp + 8]
        call effect_sprite_update
        pop esi
        ret
    }
}

/* effect_sprites_update_all */

/* "updates every active bit in both sprite sets"
 * Walks two sets of effect sprites, EFFECT_SPRITE_STRIDE bytes per sprite, and calls
 * effect_sprite_update_r for each sprite whose bit is set in its set's active bitmap.
 * Bit i lives in byte (i >> 3), mask (1 << (i & 7)) (inferred).
 * Set 1: sprites g_gfx_6c57f8 .. g_gfx_6c5df8, bitmap g_gfx_6c57b0 (inferred).
 * Set 2: sprites g_gfx_6c51b0 .. g_gfx_6c57b0, bitmap g_gfx_6c51a8 (inferred).
 * Takes no parameters and returns nothing. */

void effect_sprites_update_all(void)

{
  int bit_mask;
  undefined *sprite;
  int sprite_index;

  /* set 1: walk every sprite, update those whose bit is set in g_gfx_6c57b0 */
  sprite_index = 0;
  sprite = &g_gfx_6c57f8;
  do {
    bit_mask = 1 << ((byte)sprite_index & 7);
    if ((*(byte *)((LONG_PTR)&g_gfx_6c57b0 + (sprite_index >> 3)) & (byte)bit_mask) != 0) {
      effect_sprite_update_r(sprite,bit_mask);
    }
    sprite = sprite + EFFECT_SPRITE_STRIDE;
    sprite_index = sprite_index + 1;
  } while ((LONG_PTR)sprite < ((LONG_PTR)&g_gfx_6c5df8));
  /* set 2: same walk over the second set, bitmap g_gfx_6c51a8 */
  sprite_index = 0;
  sprite = &g_gfx_6c51b0;
  do {
    bit_mask = 1 << ((byte)sprite_index & 7);
    if ((*(byte *)((LONG_PTR)&g_gfx_6c51a8 + (sprite_index >> 3)) & (byte)bit_mask) != 0) {
      effect_sprite_update_r(sprite,bit_mask);
    }
    sprite = sprite + EFFECT_SPRITE_STRIDE;
    sprite_index = sprite_index + 1;
  } while ((LONG_PTR)sprite < ((LONG_PTR)&g_gfx_6c57b0));
  return;
}


