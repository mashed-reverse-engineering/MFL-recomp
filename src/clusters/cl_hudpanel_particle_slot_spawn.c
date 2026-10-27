// cluster-entries: hudpanel_particle_slot_spawn
/* Installed cluster hudpanel_particle_slot_spawn (tools/cluster_install.py from output/cluster_pilot_hb1/hudpanel_particle_slot_spawn.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*005dda24*/ extern float k_f_0_75;
/*005e64a4*/ extern float k_f_0;
/*00658728*/ extern float g_hudpanel_particles_658728[];
/*00658a28*/ extern float g_particles_658a28[];
/* hudpanel_particle_slot_spawn */

/* "stores the position (3 floats at in_edi) in the first free slot of the 64 particle slots (age > 0.75), else in the oldest one" */

void hudpanel_particle_slot_spawn_n(float *in_edi)

{
  float oldest;
  int slot;
  int i;
  
  for (slot = 0; slot < 0x40; slot = slot + 1) {
    if (k_f_0_75 < g_particles_658a28[slot]) {
      g_hudpanel_particles_658728[slot * 3] = in_edi[0];
      g_hudpanel_particles_658728[slot * 3 + 1] = in_edi[1];
      g_hudpanel_particles_658728[slot * 3 + 2] = in_edi[2];
      g_particles_658a28[slot] = 0.0;
      break;
    }
  }
  if (slot == 0x40) {
    slot = 0;
    oldest = k_f_0;
    for (i = 0; i < 0x40; i = i + 1) {
      if (oldest < g_particles_658a28[i]) {
        slot = i;
        oldest = g_particles_658a28[i];
      }
    }
    g_hudpanel_particles_658728[slot * 3] = in_edi[0];
    g_hudpanel_particles_658728[slot * 3 + 1] = in_edi[1];
    g_hudpanel_particles_658728[slot * 3 + 2] = in_edi[2];
    g_particles_658a28[slot] = 0.0;
  }
  return;
}

/* entry thunk at the original address of hudpanel_particle_slot_spawn: register inputs (edi) -> natural C hudpanel_particle_slot_spawn_n; ret 0 */
__declspec(naked) void hudpanel_particle_slot_spawn(void)
{
    __asm {
        push edi
        call hudpanel_particle_slot_spawn_n
        add esp, 4
        ret
    }
}
