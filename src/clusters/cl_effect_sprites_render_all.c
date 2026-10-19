// cluster-entries: effect_sprites_render_all
/* Installed cluster effect_sprites_render_all (tools/cluster_install.py from output/cluster_pilot_zx/effect_sprites_render_all.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/* DECLS-BEGIN */
/* DECLS-END */
/*006c51a8*/ extern undefined4 g_gfx_6c51a8;
/*006c51b0*/ extern undefined g_gfx_6c51b0;
/*006c57b0*/ extern undefined4 g_gfx_6c57b0;
/*006c57f8*/ extern undefined g_gfx_6c57f8;
/*006c5e10*/ extern undefined4 g_gfx_6c5e10;
/*007d2128*/ extern struct RwGlobalsRelease *RwEngineInstance;
extern char g_gfx_6c5df8[];
#define EFFECT_SPRITE_SIZE (0x60) /* bytes per effect sprite record (inferred) */
#define SPRITE_FLAG_BYTE_SHIFT (3) /* slot >> 3: 8 slot flags per byte */
#define SPRITE_FLAG_BIT_MASK (7) /* slot & 7: bit within that byte */
#define RW_ZWRITE_OFF (0x0)
#define RW_ZWRITE_ON (0x1)

/* effect_sprites_render_all
 * Draws the two effect sprite sets (16 slots each, EFFECT_SPRITE_SIZE bytes per slot).
 * A slot is drawn only when its bit is set in the set's flag bytes (inferred).
 * Z write is off while drawing, and the source/destination blend state is saved
 * first and restored after. Takes no parameters and returns nothing. Also stores
 * a camera-derived value into g_gfx_6c5e10 (inferred). */

/* "renders both sprite sets with z write off and additive blending restore" */

static int effect_sprite_set_alpha_and_render_r(int p0);
extern void effect_sprite_set_alpha_and_render(void);
void effect_sprites_render_all(void)

{
  int slot;
  undefined *sprite;
  void *saved_src_blend;
  void *saved_dest_blend;

  /* camera-derived value for the sprite code; 0 when there is no current camera */
  if (RwCameraGetCurrentCamera() == (void *)0x0) {
    g_gfx_6c5e10 = 0;
  }
  else {
    g_gfx_6c5e10 = *(int *)((char *)RwCameraGetCurrentCamera() + 4) + 0x40;
  }
  if ((g_gfx_6c57b0 != 0) || (g_gfx_6c51a8 != 0)) {
    /* save blend state, then turn z write off for the sprite draws */
    RwRenderStateGet(rwRENDERSTATESRCBLEND,&saved_src_blend);
    RwRenderStateGet(rwRENDERSTATEDESTBLEND,&saved_dest_blend);
    RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,(void *)RW_ZWRITE_OFF);
    /* first set: flags at g_gfx_6c57b0, slots start at g_gfx_6c57f8 */
    slot = 0;
    sprite = &g_gfx_6c57f8;
    do {
      if ((*(byte *)((LONG_PTR)&g_gfx_6c57b0 + (slot >> SPRITE_FLAG_BYTE_SHIFT)) & (byte)(1 << ((byte)slot & SPRITE_FLAG_BIT_MASK))) != 0) {
        effect_sprite_set_alpha_and_render_r(sprite);
      }
      sprite = sprite + EFFECT_SPRITE_SIZE;
      slot = slot + 1;
    } while ((LONG_PTR)sprite < ((LONG_PTR)&g_gfx_6c5df8));
    /* second set: flags at g_gfx_6c51a8, slots start at g_gfx_6c51b0 */
    slot = 0;
    sprite = &g_gfx_6c51b0;
    do {
      if ((*(byte *)((LONG_PTR)&g_gfx_6c51a8 + (slot >> SPRITE_FLAG_BYTE_SHIFT)) & (byte)(1 << ((byte)slot & SPRITE_FLAG_BIT_MASK))) != 0) {
        effect_sprite_set_alpha_and_render_r(sprite);
      }
      sprite = sprite + EFFECT_SPRITE_SIZE;
      slot = slot + 1;
    } while ((LONG_PTR)sprite < ((LONG_PTR)&g_gfx_6c57b0));
    RwRenderStateSet(rwRENDERSTATESRCBLEND,saved_src_blend);
    RwRenderStateSet(rwRENDERSTATEDESTBLEND,saved_dest_blend);
    RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,(void *)RW_ZWRITE_ON);
  }
  return;
}

/* effect_sprite_set_alpha_and_render_r: C -> original effect_sprite_set_alpha_and_render (registers eax) */
__declspec(naked) int effect_sprite_set_alpha_and_render_r(int p0)
{
    __asm {
        mov eax, dword ptr [esp + 4]
        call effect_sprite_set_alpha_and_render
        ret
    }
}
