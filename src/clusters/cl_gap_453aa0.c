// cluster-entries: gap_453aa0
/* Installed cluster gap_453aa0 (tools/cluster_install.py from output/cluster_pilot_hb1/gap_453aa0.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*0060d8a4*/ extern float g_course_particle_60d8a4;
/*005e64a4*/ extern float k_f_0;
/*006a1418*/ extern float g_course_particle_6a1418[];
/* gap_453aa0 */

/* WARNING: Unknown calling convention */
/* "stores the position (3 floats at in_edi) in the first free slot of the 256 course particle slots (age > threshold), else in the oldest one" */

void gap_453aa0_n(float *in_edi)

{
  float oldest;
  float *positions;
  int slot;
  int i;
  
  positions = (float *)((char *)g_course_particle_6a1418 - 0xc00);      /* the 12 byte positions (0x006a0818) precede the ages */
  for (slot = 0; slot < 0x100; slot = slot + 1) {
    if (g_course_particle_60d8a4 < g_course_particle_6a1418[slot]) {
      positions[slot * 3] = in_edi[0];
      positions[slot * 3 + 1] = in_edi[1];
      positions[slot * 3 + 2] = in_edi[2];
      g_course_particle_6a1418[slot] = 0.0;
      break;
    }
  }
  if (slot == 0x100) {
    slot = 0;
    oldest = k_f_0;
    for (i = 0; i < 0x100; i = i + 1) {
      if (oldest < g_course_particle_6a1418[i]) {
        slot = i;
        oldest = g_course_particle_6a1418[i];
      }
    }
    positions[slot * 3] = in_edi[0];
    positions[slot * 3 + 1] = in_edi[1];
    positions[slot * 3 + 2] = in_edi[2];
    g_course_particle_6a1418[slot] = 0.0;
  }
  return;
}

/* entry thunk at the original address of gap_453aa0: register inputs (edi) -> natural C gap_453aa0_n; ret 0 */
__declspec(naked) void gap_453aa0(void)
{
    __asm {
        push edi
        call gap_453aa0_n
        add esp, 4
        ret
    }
}
