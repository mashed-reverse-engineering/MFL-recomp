// cluster-entries: particles_emit_random
/* Installed cluster particles_emit_random (tools/cluster_install.py from output/cluster_pilot_hb1/particles_emit_random.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*00481fd0*/ extern double random_range(float lo, float hi);
/* particles_emit_random */

/* "fills free slots in the batch with random positions/velocities from the emitter box" */

void particles_emit_random_n(int *slot, float *box)

{
  int i;
  float10 fVar2;
  float10 fVar3;
  
  i = 0;
  do {
    if (*slot == 0) {
      *slot = 1;
      slot[1] = 0;
      fVar2 = (float10)random_range(box[4],box[7]);
      fVar3 = (float10)random_range(box[6],box[9]);
      *(float *)&slot[5] = (float)fVar2;
      fVar2 = (float10)random_range(box[5],box[8]);
      *(float *)&slot[6] = (float)fVar2;
      *(float *)&slot[7] = (float)fVar3;
      fVar2 = (float10)random_range(-box[3],box[3]);
      *(float *)&slot[2] = (float)(fVar2 + (float10)*box);
      fVar2 = (float10)random_range(-box[3],box[3]);
      *(float *)&slot[3] = (float)(fVar2 + (float10)box[1]);
      fVar2 = (float10)random_range(-box[3],box[3]);
      *(float *)&slot[4] = (float)(fVar2 + (float10)box[2]);
      return;
    }
    slot = slot + 8;
    i = i + 1;
  } while (i < 0x200);
  return;
}

/* entry thunk at the original address of particles_emit_random: register inputs (eax,esi) -> natural C particles_emit_random_n; ret 0 */
__declspec(naked) void particles_emit_random(void)
{
    __asm {
        push esi
        push eax
        call particles_emit_random_n
        add esp, 8
        ret
    }
}
