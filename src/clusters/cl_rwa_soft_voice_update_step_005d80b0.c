// cluster-entries: rwa_soft_voice_update_step_005d80b0 rwa_soft_voice_update_005d82b0
/* Installed cluster rwa_soft_voice_update_step_005d80b0 (tools/cluster_install.py from output/cluster_pilot_au5/rwa_soft_voice_update_step_005d80b0.c); entries are patched at their original addresses by build_overlay.py --clusters. */
/* Software mixer voice (RWA soft mixer): the resampling step (register function, voice in esi) and the voice's update
   callback, its only caller.  Cluster: rwa_soft_voice_update_step_005d80b0 is entered through the generated esi thunk. */
#include "mfl_tier2.h"

/*005d7e30*/ extern int rwa_soft_gain_ramp_start_005d7e30(int *ramp, int left, int right);
/*005b9630*/ extern void sub_23CD40(void *listeners, int count, const float *emitter, float *out, void *listener);      /* 3d: gain / pitch / pan of an emitter */

#define INT32_INDEFINITE (-2147483647 - 1)      /* what a 32-bit fistp stores for NaN and out-of-range values */
/* The original converts with a 32-bit fistp under chop rounding (v: a float variable). */
#define CHOP(v) (((v) >= -2147483648.0f && (v) < 2147483648.0f) ? (int)(v) : INT32_INDEFINITE)

/* voice state flags (voice+0xcc) */
#define VF_WAVE     0x01      /* wave changed */
#define VF_GAINS    0x02      /* gain / pan changed */
#define VF_PITCH    0x04      /* pitch / frequency changed */
#define VF_3D       0x10      /* 3d result changed the gains */
#define VF_KEEPPOS  0x20      /* keep the play position on a (re)start */

#define VOICE_STOPPING 1      /* voice+0x28 */
#define VOICE_STOPPED  3
#define VOICE_STARTING 2
#define VOICE_PLAYING  4

/* step = frequency * pitch / output rate as 32.32 fixed point, the low 16 fraction bits cleared.  voice+0x34 frequency (Hz),
   +0x38 pitch, +0x9c -> mixer (+0x18 output rate); the step goes to +0x80 (64 bit). */
static void FUN_005d7e80_r(char * p0);
extern void FUN_005d7e80(void);
/* TIER2: unvalidated 2026-11-05 authority=diffemu */
void rwa_soft_voice_update_step_005d80b0_n(char *voice)
{
  float rate = *(float *)(voice + 0x38) * *(float *)(voice + 0x34);
  unsigned __int64 step = ((unsigned __int64)(RwUInt32)CHOP(rate) << 32) / *(RwUInt32 *)(*(char **)(voice + 0x9c) + 0x18);

  *(unsigned __int64 *)(voice + 0x80) = step;
  *(unsigned short *)(voice + 0x80) = 0;
}

/* once per mixer update: apply what changed since the last one (wave, pitch, gains, 3d position against the listener),
   then the start / stop transitions.  Returns the voice. */
/* TIER2: unvalidated 2026-11-05 authority=diffemu */
char *rwa_soft_voice_update_005d82b0(char *voice)
{
  RwUInt32 flags = *(RwUInt32 *)(voice + 0xcc);
  RwUInt32 *vflags = (RwUInt32 *)(voice + 0xcc);
  char *wave;
  char *mixer;
  float out[6];      /* 3d result: gain, pitch, pan 0..1 (0x18 bytes in the original frame) */
  float emitter[8];      /* position, velocity, +0x64 / +0x68 of the voice */

  if (flags == 0 && *(int *)(voice + 0x28) != VOICE_STARTING && *(int *)(voice + 0x28) != VOICE_PLAYING &&
      *(int *)(voice + 0x28) != VOICE_STOPPING)
    return voice;

  if (flags & VF_WAVE) {
    wave = *(char **)(voice + 0x98);
    if (wave) {
      if (!(flags & VF_KEEPPOS)) {
        *(int *)(voice + 0x88) = 0;
        *(int *)(voice + 0x8c) = 0;
        *(int *)(voice + 0x90) = 0;
        *(int *)(voice + 0x94) = 0;
      }
      *(int *)(voice + 0x2c) = 0;
      /* frames = bytes / (bits / 8) / channels */
      *(RwUInt32 *)(voice + 0x30) = *(RwUInt32 *)(wave + 0x34) / (RwUInt32)(*(unsigned char *)(wave + 0x38) >> 3) /
                                    *(unsigned char *)(wave + 0x39);
      *(void **)(voice + 0x78) = *(void **)(wave + 0x4c);
      /* RWA then takes the wave's frequency into voice+0x34 under `if (!flags & VF_PITCH)`, which is never true: not ported */
    }
    *vflags &= ~VF_WAVE;
  }
  if (*vflags & VF_PITCH) {
    rwa_soft_voice_update_step_005d80b0_n(voice);
    *vflags &= ~VF_PITCH;
  }
  if (*vflags & VF_GAINS)
    FUN_005d7e80_r(voice);

  wave = *(char **)(voice + 0x98);
  if (wave) {
    mixer = *(char **)(voice + 0x9c);
    if (*(void **)(mixer + 0xc) && (*(unsigned char *)(wave + 0x54) & 2)) {      /* a 3d wave and a listener */
      emitter[0] = *(float *)(voice + 0x4c);
      emitter[1] = *(float *)(voice + 0x50);
      emitter[2] = *(float *)(voice + 0x54);
      emitter[3] = *(float *)(voice + 0x58);
      emitter[4] = *(float *)(voice + 0x5c);
      emitter[5] = *(float *)(voice + 0x60);
      emitter[6] = *(float *)(voice + 0x64);
      emitter[7] = *(float *)(voice + 0x68);
      sub_23CD40(mixer + 0xc, 1, emitter, out, *(void **)(mixer + 0x10));
      *(float *)(voice + 0x44) = out[0];
      *(float *)(voice + 0x38) = out[1];
      *(float *)(voice + 0x48) = out[2] + out[2] - 1.0f;
      rwa_soft_voice_update_step_005d80b0_n(voice);
      *vflags |= VF_3D;
    }
  }
  if (*vflags & VF_3D) {
    FUN_005d7e80_r(voice);
    *vflags &= ~VF_3D;
  }

  if (*(int *)(voice + 0x28) == VOICE_STARTING) {
    int *dry = (int *)(voice + 0xa4);
    int *rev = (int *)(voice + 0xb8);

    *(float *)(voice + 0x38) = 1.0f;
    *(float *)(voice + 0x44) = 1.0f;
    dry[0] = 0;
    dry[3] = 0;
    dry[4] = 0;
    dry[1] = 0;
    dry[2] = 0;
    rev[0] = 0;
    rev[3] = 0;
    rev[4] = 0;
    rev[1] = 0;
    rev[2] = 0;
    rwa_soft_gain_ramp_start_005d7e30(dry, *(int *)(voice + 0x6c), *(int *)(voice + 0x70));
    rwa_soft_gain_ramp_start_005d7e30(rev, *(int *)(voice + 0x74), *(int *)(voice + 0x74));
    *(int *)(voice + 0x28) = VOICE_PLAYING;
  } else if (*(int *)(voice + 0x28) == VOICE_STOPPING) {
    *(int *)(voice + 0x28) = VOICE_STOPPED;
    if (!(*vflags & VF_KEEPPOS)) {
      *(int *)(voice + 0x88) = 0;
      *(int *)(voice + 0x8c) = 0;
      *(int *)(voice + 0x90) = 0;
      *(int *)(voice + 0x94) = 0;
    }
  }
  *vflags &= ~VF_KEEPPOS;
  return voice;
}

/* FUN_005d7e80_r: C -> original FUN_005d7e80 (registers ecx) */
__declspec(naked) void FUN_005d7e80_r(char * p0)
{
    __asm {
        mov ecx, dword ptr [esp + 4]
        call FUN_005d7e80
        ret
    }
}

/* entry thunk at the original address of rwa_soft_voice_update_step_005d80b0: register inputs (esi) -> natural C rwa_soft_voice_update_step_005d80b0_n; ret 0 */
__declspec(naked) void rwa_soft_voice_update_step_005d80b0(void)
{
    __asm {
        push esi
        call rwa_soft_voice_update_step_005d80b0_n
        add esp, 4
        ret
    }
}
