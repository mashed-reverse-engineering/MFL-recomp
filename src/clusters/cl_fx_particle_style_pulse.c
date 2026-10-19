// cluster-entries: fx_particle_style_pulse
/* Installed cluster fx_particle_style_pulse (tools/cluster_install.py from output/cluster_pilot_zy/fx_particle_style_pulse.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*005de1f0*/ extern double k_f_0_5de1f0;
/*005de1e8*/ extern double k_f_0_5de1e8;
/*005df4c0*/ extern double k_f_0_5df4c0;
extern double sin(double x);
#pragma intrinsic(sin)

/* Per frame update of a particle's style: kinds 4 and 6 pulse the size (record word 6) with a sine of the phase (word 0xd, advanced by word 0xc) and set *out to 1.0 (kind 4) / 0.5 (kind 6); kind 0xc just sets *out to 1.0. */
void fx_particle_style_pulse_n(int *particle, unsigned int *out)
{
  float *fields = (float *)particle;

  if (particle[0] == 4) {
    fields[6] = (float)((sin(fields[0xd]) + k_f_0_5de1f0) * k_f_0_5de1e8 + k_f_0_5df4c0);
    fields[0xd] = fields[0xc] + fields[0xd];
    *out = 0x3f800000;
  } else if (particle[0] == 6) {
    fields[6] = (float)((sin(fields[0xd]) + k_f_0_5de1f0) * k_f_0_5de1e8 + k_f_0_5df4c0);
    fields[0xd] = fields[0xc] + fields[0xd];
    *out = 0x3f000000;
  } else if (particle[0] == 0xc) {
    *out = 0x3f800000;
  }
}

/* entry thunk at the original address of fx_particle_style_pulse: register inputs (eax,edx) -> natural C fx_particle_style_pulse_n; ret 0 */
__declspec(naked) void fx_particle_style_pulse(void)
{
    __asm {
        push eax
        push edx
        push edx
        push eax
        call fx_particle_style_pulse_n
        add esp, 8
        pop edx
        pop eax
        ret
    }
}
