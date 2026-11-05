// cluster-entries: rwa_soft_mix_to_pcm16_005da9f0 rwa_soft_mixer_bus_to_pcm16_005dab20
/* Installed cluster rwa_soft_mix_to_pcm16_005da9f0 (tools/cluster_install.py from output/cluster_pilot_au5/rwa_soft_mix_to_pcm16_005da9f0.c); entries are patched at their original addresses by build_overlay.py --clusters. */
/* Software mixer output (RWA soft mixer): 24.8 fixed-point mix buffer -> saturated 16-bit PCM (register function: frames
   in eax, source in edx, destination in ecx) and its only caller, which converts the dry and the reverb bus.
   Cluster: rwa_soft_mix_to_pcm16_005da9f0 is entered through the generated eax / edx / ecx thunk. */
#include "mfl_tier2.h"

/* 24.8 fixed point -> 16 bit, saturated.  (RWA compares v with -0x800000 / 0x7fff00 before the shift; comparing the
   shifted value gives the same result for every v and keeps image-range constants out of the code: objscan.) */
#define SAT16(v) (((v) >> 8) < -0x8000 ? -0x8000 : ((v) >> 8) > 0x7fff ? 0x7fff : (short)((v) >> 8))

/* frames stereo frames (2 samples each); RWA unrolls the loop by two frames */
/* TIER2: unvalidated 2026-11-05 authority=diffemu */
void rwa_soft_mix_to_pcm16_005da9f0_n(RwUInt32 frames, const int *src, short *dst)
{
  RwUInt32 pairs = frames >> 1;
  RwUInt32 i;

  for (i = pairs; i > 0; i--) {
    dst[0] = (short)SAT16(src[0]);
    dst[1] = (short)SAT16(src[1]);
    dst[2] = (short)SAT16(src[2]);
    dst[3] = (short)SAT16(src[3]);
    src += 4;
    dst += 4;
  }
  for (i = pairs * 2; i < frames; i++) {
    dst[0] = (short)SAT16(src[0]);
    dst[1] = (short)SAT16(src[1]);
    src += 2;
    dst += 2;
  }
}

/* bus+0x28 frames; +0x38 / +0x3c the dry / reverb mix, +0x2c / +0x30 their 16-bit outputs.  Returns the bus. */
/* TIER2: unvalidated 2026-11-05 authority=diffemu */
char *rwa_soft_mixer_bus_to_pcm16_005dab20(char *bus)
{
  rwa_soft_mix_to_pcm16_005da9f0_n(*(RwUInt32 *)(bus + 0x28), *(const int **)(bus + 0x38), *(short **)(bus + 0x2c));
  rwa_soft_mix_to_pcm16_005da9f0_n(*(RwUInt32 *)(bus + 0x28), *(const int **)(bus + 0x3c), *(short **)(bus + 0x30));
  return bus;
}

/* entry thunk at the original address of rwa_soft_mix_to_pcm16_005da9f0: register inputs (eax,edx,ecx) -> natural C rwa_soft_mix_to_pcm16_005da9f0_n; ret 0 */
__declspec(naked) void rwa_soft_mix_to_pcm16_005da9f0(void)
{
    __asm {
        push ecx
        push edx
        push eax
        call rwa_soft_mix_to_pcm16_005da9f0_n
        add esp, 12
        ret
    }
}
