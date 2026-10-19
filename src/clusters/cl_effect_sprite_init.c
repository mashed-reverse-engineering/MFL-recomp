// cluster-entries: effect_sprite_init
/* Installed cluster effect_sprite_init (tools/cluster_install.py from output/cluster_pilot10/effect_sprite_init.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*006c51a8*/ extern undefined4 g_gfx_6c51a8;
/*006c57b0*/ extern undefined4 g_gfx_6c57b0;
/* effect_sprite_init */

/* WARNING: Unknown calling convention */
/* "copies a 4x4 matrix and a colour into the sprite entry and sets its active bit in g_gfx_6c57b0"
    */

void effect_sprite_init_n(undefined4 *in_eax, undefined4 *param_2, undefined4 *param_3, undefined4 param_4, undefined4 param_5)

{
  byte *pbVar1;
  int iVar2;
  undefined4 *puVar3;
  
  in_eax[0x13] = param_4;
  in_eax[0x14] = param_5;
  in_eax[0x15] = 0;
  in_eax[0x12] = 0;
  puVar3 = in_eax;
  for (iVar2 = 0x10; iVar2 != 0; iVar2 = iVar2 + -1) {
    *puVar3 = *param_2;
    param_2 = param_2 + 1;
    puVar3 = puVar3 + 1;
  }
  in_eax[0x10] = *param_3;
  if (in_eax[0x17] == 0) {
    iVar2 = (int)in_eax[0x16] >> 3;
    *(byte *)((char *)&g_gfx_6c57b0 + iVar2) =
         *(byte *)((char *)&g_gfx_6c57b0 + iVar2) | '\x01' << ((byte)in_eax[0x16] & 7);
    return;
  }
  if (in_eax[0x17] == 1) {
    pbVar1 = (byte *)((int)&g_gfx_6c51a8 + ((int)in_eax[0x16] >> 3));
    *pbVar1 = *pbVar1 | '\x01' << ((byte)in_eax[0x16] & 7);
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
