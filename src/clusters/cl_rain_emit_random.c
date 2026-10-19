// cluster-entries: rain_emit_random
/* Installed cluster rain_emit_random (tools/cluster_install.py from output/cluster_pilot_hb1/rain_emit_random.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*00481fd0*/ extern double random_range(float lo, float hi);
/* rain_emit_random */

/* WARNING: Unknown calling convention */
/* "fills free rain slots with random positions/velocities from the emitter box" */

void rain_emit_random_n(int *in_eax, float *in_edi)

{
  int iVar1;
  float10 fVar2;
  float10 fVar3;
  
  iVar1 = 0;
  do {
    if (*in_eax == 0) {
      *in_eax = 1;
      in_eax[1] = 0;
      fVar2 = (float10)random_range(in_edi[4],in_edi[7]);
      fVar3 = (float10)random_range(in_edi[6],in_edi[9]);
      *(float *)&in_eax[5] = (float)fVar2;
      fVar2 = (float10)random_range(in_edi[5],in_edi[8]);
      *(float *)&in_eax[6] = (float)fVar2;
      *(float *)&in_eax[7] = (float)fVar3;
      fVar2 = (float10)random_range(-in_edi[3],in_edi[3]);
      *(float *)&in_eax[2] = (float)(fVar2 + (float10)*in_edi);
      in_eax[3] = *(int *)&in_edi[1];
      fVar2 = (float10)random_range(-in_edi[3],in_edi[3]);
      *(float *)&in_eax[4] = (float)(fVar2 + (float10)in_edi[2]);
      return;
    }
    in_eax = in_eax + 8;
    iVar1 = iVar1 + 1;
  } while (iVar1 < 0x200);
  return;
}

/* entry thunk at the original address of rain_emit_random: register inputs (eax,edi) -> natural C rain_emit_random_n; ret 0 */
__declspec(naked) void rain_emit_random(void)
{
    __asm {
        push edi
        push eax
        call rain_emit_random_n
        add esp, 8
        ret
    }
}
