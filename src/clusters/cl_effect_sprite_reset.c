// cluster-entries: effect_sprite_reset
/* Installed cluster effect_sprite_reset (tools/cluster_install.py from output/cluster_pilot10/effect_sprite_reset.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*006c51a8*/ extern undefined4 g_gfx_6c51a8;
/*006c57b0*/ extern undefined4 g_gfx_6c57b0;
/* effect_sprite_reset */

/* WARNING: Unknown calling convention */
/* "resets a sprite entry: colour white, scale 1, flags |= 0x20003" */

void effect_sprite_reset_n(undefined4 *in_eax)

{
  byte *pbVar1;
  
  *(undefined1 *)(in_eax + 0x10) = 0xff;
  *(undefined1 *)((char *)in_eax + 0x41) = 0xff;
  *(undefined1 *)((char *)in_eax + 0x42) = 0xff;
  *(undefined1 *)((char *)in_eax + 0x43) = 0xff;
  in_eax[10] = 0x3f800000;
  in_eax[5] = 0x3f800000;
  *in_eax = 0x3f800000;
  in_eax[3] = in_eax[3] | 0x20003;
  in_eax[0x12] = 0;
  in_eax[0x13] = 0;
  in_eax[0x14] = 0;
  in_eax[0x15] = 0;
  in_eax[4] = 0;
  in_eax[2] = 0;
  in_eax[1] = 0;
  in_eax[9] = 0;
  in_eax[8] = 0;
  in_eax[6] = 0;
  in_eax[0xe] = 0;
  in_eax[0xd] = 0;
  in_eax[0xc] = 0;
  if (in_eax[0x17] == 0) {
    pbVar1 = (byte *)((int)&g_gfx_6c57b0 + ((int)in_eax[0x16] >> 3));
    *pbVar1 = *pbVar1 & ~('\x01' << ((byte)in_eax[0x16] & 7));
    return;
  }
  if (in_eax[0x17] == 1) {
    pbVar1 = (byte *)((int)&g_gfx_6c51a8 + ((int)in_eax[0x16] >> 3));
    *pbVar1 = *pbVar1 & ~('\x01' << ((byte)in_eax[0x16] & 7));
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
