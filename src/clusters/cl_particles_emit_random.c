// cluster-entries: particles_emit_random
/* Installed cluster particles_emit_random (tools/cluster_install.py from output/cluster_pilot_hb1/particles_emit_random.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*00481fd0*/ extern double random_range(float lo, float hi);
/* particles_emit_random */

/* WARNING: Unknown calling convention */
/* "fills free slots in the batch with random positions/velocities from the emitter box" */

void particles_emit_random_n(int *in_eax, float *in_esi)

{
  int iVar1;
  float10 fVar2;
  float10 fVar3;
  
  iVar1 = 0;
  do {
    if (*in_eax == 0) {
      *in_eax = 1;
      in_eax[1] = 0;
      fVar2 = (float10)random_range(in_esi[4],in_esi[7]);
      fVar3 = (float10)random_range(in_esi[6],in_esi[9]);
      *(float *)&in_eax[5] = (float)fVar2;
      fVar2 = (float10)random_range(in_esi[5],in_esi[8]);
      *(float *)&in_eax[6] = (float)fVar2;
      *(float *)&in_eax[7] = (float)fVar3;
      fVar2 = (float10)random_range(-in_esi[3],in_esi[3]);
      *(float *)&in_eax[2] = (float)(fVar2 + (float10)*in_esi);
      fVar2 = (float10)random_range(-in_esi[3],in_esi[3]);
      *(float *)&in_eax[3] = (float)(fVar2 + (float10)in_esi[1]);
      fVar2 = (float10)random_range(-in_esi[3],in_esi[3]);
      *(float *)&in_eax[4] = (float)(fVar2 + (float10)in_esi[2]);
      return;
    }
    in_eax = in_eax + 8;
    iVar1 = iVar1 + 1;
  } while (iVar1 < 0x200);
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
