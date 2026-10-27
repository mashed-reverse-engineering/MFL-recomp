// cluster-entries: rain_emit_random
/* Installed cluster rain_emit_random (tools/cluster_install.py from output/cluster_pilot_hb1/rain_emit_random.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*00481fd0*/ extern double random_range(float lo, float hi);
/* rain_emit_random */

/* "fills free rain slots with random positions/velocities from the emitter box" */

void rain_emit_random_n(int *drop, float *box)

{
  int i;
  float10 fVar2;
  float10 fVar3;
  
  i = 0;
  do {
    if (*drop == 0) {
      *drop = 1;
      drop[1] = 0;
      fVar2 = (float10)random_range(box[4],box[7]);
      fVar3 = (float10)random_range(box[6],box[9]);
      *(float *)&drop[5] = (float)fVar2;
      fVar2 = (float10)random_range(box[5],box[8]);
      *(float *)&drop[6] = (float)fVar2;
      *(float *)&drop[7] = (float)fVar3;
      fVar2 = (float10)random_range(-box[3],box[3]);
      *(float *)&drop[2] = (float)(fVar2 + (float10)*box);
      drop[3] = *(int *)&box[1];
      fVar2 = (float10)random_range(-box[3],box[3]);
      *(float *)&drop[4] = (float)(fVar2 + (float10)box[2]);
      return;
    }
    drop = drop + 8;
    i = i + 1;
  } while (i < 0x200);
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
