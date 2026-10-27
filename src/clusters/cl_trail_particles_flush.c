// cluster-entries: trail_particles_flush
/* Installed cluster trail_particles_flush (tools/cluster_install.py from output/cluster_pilot_zx/trail_particles_flush.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/* DECLS-BEGIN */
/* DECLS-END */
/*00653160*/ extern undefined4 g_hud_653160;
/*008cb500*/ extern undefined g_hud_8cb500;
/*008d8b00*/ extern undefined4 g_trail_8d8b00;
#define TRAIL_SLOT_STRIDE (0x2c)

/* trail_particles_flush */

/* "adds the pending particles"
 * Resets g_trail_8d8b00, then calls trail_particle_add once for each trail slot
 * in g_hud_8cb500 (slot count in g_hud_653160; slots are TRAIL_SLOT_STRIDE bytes apart).
 * particle_arg is passed on as the eax argument of trail_particle_add (meaning inferred).
 * Returns nothing. */

static int trail_particle_add_r(int p0, int p1, float p2);
extern void trail_particle_add(void);
void trail_particles_flush(undefined4 particle_arg)

{
  int slot_index;
  undefined *slot;

  slot_index = 0;
  g_trail_8d8b00 = 0;
  if (0 < (int)g_hud_653160) {
    /* walk the slot array, one trail_particle_add call per slot */
    slot = &g_hud_8cb500;
    do {
      trail_particle_add_r(slot,particle_arg,1.6);
      slot_index = slot_index + 1;
      slot = slot + TRAIL_SLOT_STRIDE;
    } while (slot_index < (int)g_hud_653160);
  }
  return;
}

/* trail_particle_add_r: C -> original trail_particle_add (registers edi,eax,s0) */
__declspec(naked) int trail_particle_add_r(int p0, int p1, float p2)
{
    __asm {
        push edi
        push dword ptr [esp + 16]
        mov edi, dword ptr [esp + 12]
        mov eax, dword ptr [esp + 16]
        call trail_particle_add
        add esp, 4
        pop edi
        ret
    }
}
