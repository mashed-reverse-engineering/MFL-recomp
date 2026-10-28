// cluster-entries: effect_sprite_reset
/* Installed cluster effect_sprite_reset (tools/cluster_install.py from output/cluster_pilot10/effect_sprite_reset.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*006c51a8*/ extern undefined4 g_gfx_6c51a8;
/*006c57b0*/ extern undefined4 g_gfx_6c57b0;
/* effect_sprite_reset */

/* "resets a sprite entry: colour white, scale 1, flags |= 0x20003" */

/* Reset sprite entry: colour white (alpha/rgb 0xff), scale 1.0f, clear transform/uv fields, set flags |= 0x20003, then clear its bit in the allocation bitmap selected by sprite[0x17] (0 -> g_gfx_6c57b0, 1 -> g_gfx_6c51a8; index sprite[0x16]). (inferred) */
void effect_sprite_reset_n(undefined4 *sprite)

{
  byte *bitmap_byte;
  
  *(undefined1 *)(sprite + 0x10) = 0xff;
  *(undefined1 *)((char *)sprite + 0x41) = 0xff;
  *(undefined1 *)((char *)sprite + 0x42) = 0xff;
  *(undefined1 *)((char *)sprite + 0x43) = 0xff;
  sprite[10] = 0x3f800000;
  sprite[5] = 0x3f800000;
  *sprite = 0x3f800000;
  sprite[3] = sprite[3] | 0x20003;
  sprite[0x12] = 0;
  sprite[0x13] = 0;
  sprite[0x14] = 0;
  sprite[0x15] = 0;
  sprite[4] = 0;
  sprite[2] = 0;
  sprite[1] = 0;
  sprite[9] = 0;
  sprite[8] = 0;
  sprite[6] = 0;
  sprite[0xe] = 0;
  sprite[0xd] = 0;
  sprite[0xc] = 0;
  if (sprite[0x17] == 0) {
    bitmap_byte = (byte *)((int)&g_gfx_6c57b0 + ((int)sprite[0x16] >> 3));
    *bitmap_byte = *bitmap_byte & ~('\x01' << ((byte)sprite[0x16] & 7));
    return;
  }
  if (sprite[0x17] == 1) {
    bitmap_byte = (byte *)((int)&g_gfx_6c51a8 + ((int)sprite[0x16] >> 3));
    *bitmap_byte = *bitmap_byte & ~('\x01' << ((byte)sprite[0x16] & 7));
  }
  return;
}

/* entry thunk at the original address of effect_sprite_reset: register inputs (eax) -> natural C effect_sprite_reset_n; ret 0 */
__declspec(naked) void effect_sprite_reset(void)
{
    __asm {
        push eax
        call effect_sprite_reset_n
        add esp, 4
        ret
    }
}
