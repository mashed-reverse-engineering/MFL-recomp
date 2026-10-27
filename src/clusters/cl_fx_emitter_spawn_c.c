// cluster-entries: fx_emitter_spawn_c
/* Installed cluster fx_emitter_spawn_c (tools/cluster_install.py from output/cluster_pilot_zx/fx_emitter_spawn_c.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/* DECLS-BEGIN */
/* DECLS-END */
/*006c4304*/ extern undefined4 g_gfx_6c4304;
extern char g_fx_emitter_6c51c4[];

/* ints per emitter block in the emitter table */
#define EMITTER_STRIDE (0x3b)
/* ints per group of 8 emitters (8 * EMITTER_STRIDE) */
#define SLOT_GROUP_STRIDE (0x1d8)


/* fx_emitter_spawn_c */

/* "finds a free emitter slot and initialises it (variant c)"
 *
 * Scans the emitter table in groups of 8 emitters (SLOT_GROUP_STRIDE ints apart), starting at
 * g_gfx_6c4304, and picks the first emitter whose flag is 0 (inferred: 0 = free). Then calls
 * fx_emitter_init with that emitter's address in edi and pos as the position (inferred).
 * If no emitter is free, the scan stops at the end of the table and uses the last candidate.
 * param_2..param_5 are passed through unchanged (meaning unknown).
 */

static int fx_emitter_init_r(int p0, float * p1, undefined4 p2, undefined4 p3, float p4, int p5, undefined4 p6);
extern void fx_emitter_init(void);
void __cdecl fx_emitter_spawn_c(float *pos,undefined4 param_2,undefined4 param_3,float param_4,int param_5)

{
  int *slot;
  int *emitter;

  slot = &g_gfx_6c4304;
  /* scan groups: break at the first free emitter, else stop at the end of the table */
  do {
    emitter = slot + -0x71;
    if (((((slot[-EMITTER_STRIDE] == 0) || (emitter = slot + -0x36, *slot == 0)) ||
         (emitter = slot + 5, slot[EMITTER_STRIDE] == 0)) ||
        ((emitter = slot + 0x40, slot[0x76] == 0 || (emitter = slot + 0x7b, slot[0xb1] == 0)
         ))) || ((emitter = slot + 0xb6, slot[0xec] == 0 ||
                 ((emitter = slot + 0xf1, slot[0x127] == 0 ||
                  (emitter = slot + 300, slot[0x162] == 0)))))) break;
    slot = slot + SLOT_GROUP_STRIDE;
  } while ((LONG_PTR)slot < ((LONG_PTR)&g_fx_emitter_6c51c4));
  fx_emitter_init_r(emitter,pos,param_2,param_3,param_4,param_5,0);
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
