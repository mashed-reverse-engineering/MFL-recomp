#include "mfl_tier2.h"
/*005ddb38*/ extern float k_f_25;
/*006294a4*/ extern undefined g_gfx_6294a4;
/*006c51a8*/ extern undefined4 g_gfx_6c51a8;
/*006c51b0*/ extern undefined g_gfx_6c51b0;
/*006c57b0*/ extern undefined4 g_gfx_6c57b0;
/*006c57b8*/ extern undefined4 g_gfx_6c57b8;
/*006c57f8*/ extern undefined g_gfx_6c57f8;
/*006c5e10*/ extern float *g_gfx_6c5e10;
static void effect_sprite_init_r(int sprite, void *position, void *matrix, int extent_a, int extent_b);
extern void effect_sprite_init(void);
/* adapter for the register convention callee 0x004872e0: eax = sprite slot, 4 stack arguments */
__declspec(naked) void effect_sprite_init_r(int sprite, void *position, void *matrix, int extent_a, int extent_b)
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

void effect_sprite_spawn(int kind,undefined4 *matrix,undefined4 *colour,undefined4 extent_a,
                        undefined4 extent_b)

{
  int slot;
  undefined *sprite;
  
  if (matrix == (undefined4 *)0x0) {
    matrix = (undefined4 *)&g_gfx_6c57b8;
  }
  if (colour == (undefined4 *)0x0) {
    colour = (undefined4 *)&g_gfx_6294a4;
  }
  if ((g_gfx_6c5e10 == (float *)0x0) ||
     !((g_gfx_6c5e10[2] - *(float *)&matrix[0xe]) * (g_gfx_6c5e10[2] - *(float *)&matrix[0xe]) +
                (*g_gfx_6c5e10 - *(float *)&matrix[0xc]) * (*g_gfx_6c5e10 - *(float *)&matrix[0xc]) +
                (g_gfx_6c5e10[1] - *(float *)&matrix[0xd]) * (g_gfx_6c5e10[1] - *(float *)&matrix[0xd]) < k_f_25))
  {
    slot = 0;
    if (kind == 0) {
      while ((*(byte *)((char *)&g_gfx_6c57b0 + (slot >> 3)) & (byte)(1 << ((byte)slot & 7))) != 0) {
        slot = slot + 1;
        if (0xf < slot) {
          return;
        }
      }
      sprite = &g_gfx_6c57f8 + slot * 0x60;
    }
    else {
      if (kind != 1) {
        return;
      }
      slot = 0;
      while ((*(byte *)((char *)&g_gfx_6c51a8 + (slot >> 3)) & (byte)(1 << ((byte)slot & 7))) != 0) {
        slot = slot + 1;
        if (0xf < slot) {
          return;
        }
      }
      sprite = &g_gfx_6c51b0 + slot * 0x60;
    }
    effect_sprite_init_r((int)sprite,matrix,colour,extent_a,extent_b);
  }
  return;
}


