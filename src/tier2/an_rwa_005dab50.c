/* rwa_soft_voice_mix_005dab50 (0x005dab50): software mixer: add one playing voice's resampled block to the dry and reverb buses */
#include "mfl_tier2.h"

#define SOFT_STATE_STOPPING 1      /* state+0x00 */
#define SOFT_STATE_IDLE     3
#define SOFT_STATE_PLAYING  4

/* voice+0x20 -> mix state st:
     +0x00 state, +0x44 / +0x48 left / right gain (8.8), +0x4c reverb send (8.8), +0x54 resampled 16-bit block,
     +0x70 -> wave format (+0x39 channels), +0x74 -> bus (+0x28 frames, +0x38 dry, +0x3c reverb: interleaved stereo int32),
     +0x7c ramp frames left, +0x80 / +0x84 ramped left / right gain and +0x88 / +0x8c their steps,
     +0x90 reverb ramp frames, +0x94 / +0x98 ramped reverb sends (16.16 of the ramped dry sample) and +0x9c / +0xa0 their steps.
   The order of the loads and stores follows the original exactly (the fields are re-read after every bus store).
   Quirks kept from the original: the mono tail after the 4-frame loop is bus frames & 3 counted from the whole block (not
   from what the ramp left over), and the stereo ramp STORES into the buses instead of adding. Returns the voice, or NULL
   when it is not playing. */
/* TIER2: unvalidated 2026-11-05 authority=diffemu */
char *rwa_soft_voice_mix_005dab50(char *voice)
{
  char *st = *(char **)(voice + 0x20);
  char *bus = *(char **)(st + 0x74);
  int *dry, *wet;
  short *src;
  int gain_l, gain_r, s, l, r;
  RwUInt32 n, k;

  if (*(int *)st != SOFT_STATE_PLAYING)
    return 0;
  dry = *(int **)(bus + 0x38);
  gain_l = *(int *)(st + 0x44);
  gain_r = *(int *)(st + 0x48);
  wet = *(int **)(bus + 0x3c);
  n = *(RwUInt32 *)(bus + 0x28);
  src = *(short **)(st + 0x54);

  if ((*(unsigned char **)(st + 0x70))[0x39] == 1) {      /* mono source */
    if (*(int *)(st + 0x7c) != 0) {
      while (n != 0) {
        s = src[0];
        l = (*(int *)(st + 0x80) * s) >> 8;
        r = (*(int *)(st + 0x84) * s) >> 8;
        dry[0] += l;
        dry[1] += r;
        wet[0] += (*(int *)(st + 0x94) * l) >> 16;
        wet[1] += (*(int *)(st + 0x98) * r) >> 16;
        *(int *)(st + 0x80) += *(int *)(st + 0x88);
        *(int *)(st + 0x84) += *(int *)(st + 0x8c);
        src++;
        dry += 2;
        wet += 2;
        *(int *)(st + 0x7c) -= 1;
        *(int *)(st + 0x94) += *(int *)(st + 0x9c);
        *(int *)(st + 0x98) += *(int *)(st + 0xa0);
        *(int *)(st + 0x90) -= 1;
        n--;
        if (*(int *)(st + 0x7c) == 0)
          break;
      }
    }
    for (k = n >> 2; k != 0; k--) {
      int i;

      for (i = 0; i < 4; i++) {
        l = src[i] * gain_l;
        r = src[i] * gain_r;
        dry[2 * i] += l;
        dry[2 * i + 1] += r;
        wet[2 * i] += (*(int *)(st + 0x4c) * l) >> 8;
        wet[2 * i + 1] += (*(int *)(st + 0x4c) * r) >> 8;
      }
      src += 4;
      dry += 8;
      wet += 8;
    }
    for (k = *(RwUInt32 *)(bus + 0x28) & 3; k != 0; k--) {
      l = src[0] * gain_l;
      r = src[0] * gain_r;
      dry[0] += l;
      dry[1] += r;
      wet[0] += (*(int *)(st + 0x4c) * l) >> 8;
      wet[1] += (*(int *)(st + 0x4c) * r) >> 8;
      src++;
      dry += 2;
      wet += 2;
    }
  } else {      /* stereo source */
    if (*(int *)(st + 0x7c) != 0) {
      for (;;) {
        if (n == 0)
          goto done;
        l = (src[0] * *(int *)(st + 0x80)) >> 8;
        r = (src[1] * *(int *)(st + 0x84)) >> 8;
        dry[0] = l;
        wet[0] = (*(int *)(st + 0x94) * l) >> 16;
        dry[1] = r;
        wet[1] = (*(int *)(st + 0x98) * r) >> 16;
        *(int *)(st + 0x80) += *(int *)(st + 0x88);
        *(int *)(st + 0x84) += *(int *)(st + 0x8c);
        src += 2;
        dry += 2;
        wet += 2;
        *(int *)(st + 0x7c) -= 1;
        *(int *)(st + 0x94) += *(int *)(st + 0x9c);
        *(int *)(st + 0x98) += *(int *)(st + 0xa0);
        *(int *)(st + 0x90) -= 1;
        n--;
        if (*(int *)(st + 0x7c) == 0)
          break;
      }
    }
    for (; n != 0; n--) {
      l = src[0] * gain_l;
      r = src[1] * gain_r;
      dry[0] += l;
      dry[1] += r;
      wet[0] += (*(int *)(st + 0x4c) * l) >> 8;
      wet[1] += (*(int *)(st + 0x4c) * r) >> 8;
      src += 2;
      dry += 2;
      wet += 2;
    }
  }
done:
  if (*(int *)st == SOFT_STATE_STOPPING) {
    *(int *)st = SOFT_STATE_IDLE;
    *(int *)(st + 0x60) = 0;
    *(int *)(st + 0x64) = 0;
  }
  return voice;
}
