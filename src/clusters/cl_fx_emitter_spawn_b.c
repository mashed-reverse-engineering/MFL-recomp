// cluster-entries: fx_emitter_spawn_b
/* Installed cluster fx_emitter_spawn_b (tools/cluster_install.py from output/cluster_pilot_zx/fx_emitter_spawn_b.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/* DECLS-BEGIN */
/* DECLS-END */
/*006c4304*/ extern undefined4 g_gfx_6c4304;
extern char g_fx_emitter_6c51c4[];
#define EMITTER_SLOT_STRIDE (0x1d8) /* ints per emitter slot (inferred) */

/* fx_emitter_spawn_b */

/* "finds a free emitter slot and initialises it (variant b, with scale)" */

/* Spawns an emitter at pos with the given scale; pos and the rest are passed on to fx_emitter_init.
   Walks the emitter slots and picks the first one whose sub-emitter flag words are zero (inferred).
   If none is free, the loop ends and the last candidate pointer is used. (inferred) */
static int fx_emitter_init_r(int p0, float * p1, undefined4 p2, undefined4 p3, float p4, int p5, undefined4 p6);
extern void fx_emitter_init(void);
void __cdecl fx_emitter_spawn_b(float *pos,undefined4 param_2,undefined4 param_3,float scale,int param_5)

{
  int *slot;
  int *free_slot;

  /* scan slots in EMITTER_SLOT_STRIDE steps; break on the first free one */
  slot = &g_gfx_6c4304;
  do {
    free_slot = slot + -0x71;
    if (((((slot[-0x3b] == 0) || (free_slot = slot + -0x36, *slot == 0)) ||
         (free_slot = slot + 5, slot[0x3b] == 0)) ||
        ((free_slot = slot + 0x40, slot[0x76] == 0 || (free_slot = slot + 0x7b, slot[0xb1] == 0)
         ))) || ((free_slot = slot + 0xb6, slot[0xec] == 0 ||
                 ((free_slot = slot + 0xf1, slot[0x127] == 0 ||
                  (free_slot = slot + 300, slot[0x162] == 0)))))) break;
    slot = slot + EMITTER_SLOT_STRIDE;
  } while ((LONG_PTR)slot < ((LONG_PTR)&g_fx_emitter_6c51c4));
  fx_emitter_init_r(free_slot,pos,param_2,param_3,scale,param_5,1);
  return;
}

/* fx_emitter_init_r: C -> original fx_emitter_init (registers edi,s0,s1,s2,s3,s4,s5) */
__declspec(naked) int fx_emitter_init_r(int p0, float * p1, undefined4 p2, undefined4 p3, float p4, int p5, undefined4 p6)
{
    __asm {
        push edi
        push dword ptr [esp + 32]
        push dword ptr [esp + 32]
        push dword ptr [esp + 32]
        push dword ptr [esp + 32]
        push dword ptr [esp + 32]
        push dword ptr [esp + 32]
        mov edi, dword ptr [esp + 32]
        call fx_emitter_init
        add esp, 24
        pop edi
        ret
    }
}
