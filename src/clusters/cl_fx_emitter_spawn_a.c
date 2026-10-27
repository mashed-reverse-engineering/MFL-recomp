// cluster-entries: fx_emitter_spawn_a
/* Installed cluster fx_emitter_spawn_a (tools/cluster_install.py from output/cluster_pilot_zx/fx_emitter_spawn_a.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/* DECLS-BEGIN */
/* DECLS-END */
/*006c4304*/ extern undefined4 g_gfx_6c4304;
extern char g_fx_emitter_6c51c4[];

/* ints from one emitter slot to the next in the g_gfx_6c4304 table */
#define FX_EMITTER_SLOT_INTS (0x1d8)
/* bit pattern of 1.0f, passed to fx_emitter_init as an int */
#define FLOAT_ONE_BITS (0x3f800000)


/* fx_emitter_spawn_a */

/* "finds a free emitter slot and initialises it (variant a)"
 * Walks the emitter table at g_gfx_6c4304 one slot at a time and stops at the first
 * slot where one of its eight flag words is zero (inferred: flag word == 0 means free).
 * free_record is the record that goes with that flag word. If every slot is in use,
 * the walk stops at the end of the table and free_record is the last record checked (inferred).
 * Then fx_emitter_init initialises that record.
 * pos: emitter position (inferred); param_2, param_3: forwarded unchanged, meaning unknown;
 * scale: forwarded as the float argument (inferred).
 */

static int fx_emitter_init_r(int p0, float * p1, undefined4 p2, undefined4 p3, float p4, int p5, undefined4 p6);
extern void fx_emitter_init(void);
void __cdecl fx_emitter_spawn_a(float *pos,undefined4 param_2,undefined4 param_3,float scale)

{
  int *slot;
  int *free_record;
  
  slot = &g_gfx_6c4304;
  /* loop: test the flag words of this slot; the first zero one selects free_record and breaks */
  do {
    free_record = slot + -0x71;
    if (((((slot[-0x3b] == 0) || (free_record = slot + -0x36, *slot == 0)) ||
         (free_record = slot + 5, slot[0x3b] == 0)) ||
        ((free_record = slot + 0x40, slot[0x76] == 0 || (free_record = slot + 0x7b, slot[0xb1] == 0)
         ))) || ((free_record = slot + 0xb6, slot[0xec] == 0 ||
                 ((free_record = slot + 0xf1, slot[0x127] == 0 ||
                  (free_record = slot + 300, slot[0x162] == 0)))))) break;
    slot = slot + FX_EMITTER_SLOT_INTS;
  } while ((LONG_PTR)slot < ((LONG_PTR)&g_fx_emitter_6c51c4));
  fx_emitter_init_r(free_record,pos,param_2,param_3,scale,FLOAT_ONE_BITS,1);
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
