#include "mfl_tier2.h"
/*005ddb38*/ extern float k_f_25;
/*006294a4*/ extern undefined g_gfx_6294a4;
/*006c51a8*/ extern undefined4 g_gfx_6c51a8;
/*006c51b0*/ extern undefined g_gfx_6c51b0;
/*006c57b0*/ extern undefined4 g_gfx_6c57b0;
/*006c57b8*/ extern undefined4 g_gfx_6c57b8;
/*006c57f8*/ extern undefined g_gfx_6c57f8;
/*006c5e10*/ extern float *g_gfx_6c5e10;
static void effect_sprite_init_r(int reg_eax, void *position, void *matrix, int param_4, int param_5);
extern void effect_sprite_init(void);
/* adapter for the register convention callee 0x004872e0: eax = sprite slot, 4 stack arguments */
__declspec(naked) void effect_sprite_init_r(int reg_eax, void *position, void *matrix, int param_4, int param_5)
{
    __asm {
        push dword ptr [esp + 0x14]
        push dword ptr [esp + 0x14]
        push dword ptr [esp + 0x14]
        push dword ptr [esp + 0x14]
        mov eax, dword ptr [esp + 0x14]
        call effect_sprite_init
        add esp, 0x10
        ret
    }
}

/* effect_sprite_spawn */

/* "allocates a sprite near the listener (within distance 5) with default matrices" */

void effect_sprite_spawn(int param_1,undefined4 *param_2,undefined4 *param_3,undefined4 param_4,
                        undefined4 param_5)

{
  int iVar1;
  undefined *reg_eax;
  
  if (param_2 == (undefined4 *)0x0) {
    param_2 = (undefined4 *)&g_gfx_6c57b8;
  }
  if (param_3 == (undefined4 *)0x0) {
    param_3 = (undefined4 *)&g_gfx_6294a4;
  }
  if ((g_gfx_6c5e10 == (float *)0x0) ||
     (k_f_25 <= (g_gfx_6c5e10[2] - *(float *)&param_2[0xe]) * (g_gfx_6c5e10[2] - *(float *)&param_2[0xe]) +
                (*g_gfx_6c5e10 - *(float *)&param_2[0xc]) * (*g_gfx_6c5e10 - *(float *)&param_2[0xc]) +
                (g_gfx_6c5e10[1] - *(float *)&param_2[0xd]) * (g_gfx_6c5e10[1] - *(float *)&param_2[0xd])))
  {
    iVar1 = 0;
    if (param_1 == 0) {
      while ((*(byte *)((char *)&g_gfx_6c57b0 + (iVar1 >> 3)) & (byte)(1 << ((byte)iVar1 & 7))) != 0) {
        iVar1 = iVar1 + 1;
        if (0xf < iVar1) {
          return;
        }
      }
      reg_eax = &g_gfx_6c57f8 + iVar1 * 0x60;
    }
    else {
      if (param_1 != 1) {
        return;
      }
      iVar1 = 0;
      while ((*(byte *)((char *)&g_gfx_6c51a8 + (iVar1 >> 3)) & (byte)(1 << ((byte)iVar1 & 7))) != 0) {
        iVar1 = iVar1 + 1;
        if (0xf < iVar1) {
          return;
        }
      }
      reg_eax = &g_gfx_6c51b0 + iVar1 * 0x60;
    }
    effect_sprite_init_r((int)reg_eax,param_2,param_3,param_4,param_5);
  }
  return;
}


