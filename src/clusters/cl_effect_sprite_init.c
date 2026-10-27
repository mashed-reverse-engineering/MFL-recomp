// cluster-entries: effect_sprite_init
/* Installed cluster effect_sprite_init (tools/cluster_install.py from output/cluster_pilot10/effect_sprite_init.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*006c51a8*/ extern undefined4 g_gfx_6c51a8;
/*006c57b0*/ extern undefined4 g_gfx_6c57b0;
/* effect_sprite_init */

/* WARNING: Unknown calling convention */
/* "copies a 4x4 matrix and a colour into the sprite entry and sets its active bit in g_gfx_6c57b0"
    */

void effect_sprite_init_n(undefined4 *sprite, undefined4 *src_matrix, undefined4 *src_colour, undefined4 extent_a, undefined4 extent_b)

{
  byte *bit_byte;
  int count_or_byte;
  undefined4 *dst_cursor;
  
  sprite[0x13] = extent_a;
  sprite[0x14] = extent_b;
  sprite[0x15] = 0;
  sprite[0x12] = 0;
  dst_cursor = sprite;
  for (count_or_byte = 0x10; count_or_byte != 0; count_or_byte = count_or_byte + -1) {
    *dst_cursor = *src_matrix;
    src_matrix = src_matrix + 1;
    dst_cursor = dst_cursor + 1;
  }
  sprite[0x10] = *src_colour;
  if (sprite[0x17] == 0) {
    count_or_byte = (int)sprite[0x16] >> 3;
    *(byte *)((char *)&g_gfx_6c57b0 + count_or_byte) =
         *(byte *)((char *)&g_gfx_6c57b0 + count_or_byte) | '\x01' << ((byte)sprite[0x16] & 7);
    return;
  }
  if (sprite[0x17] == 1) {
    bit_byte = (byte *)((int)&g_gfx_6c51a8 + ((int)sprite[0x16] >> 3));
    *bit_byte = *bit_byte | '\x01' << ((byte)sprite[0x16] & 7);
  }
  return;
}

/* entry thunk at the original address of effect_sprite_init: register inputs (eax,s0,s1,s2,s3) -> natural C effect_sprite_init_n; ret 0 */
__declspec(naked) void effect_sprite_init(void)
{
    __asm {
        push dword ptr [esp + 16]
        push dword ptr [esp + 16]
        push dword ptr [esp + 16]
        push dword ptr [esp + 16]
        push eax
        call effect_sprite_init_n
        add esp, 20
        ret
    }
}
