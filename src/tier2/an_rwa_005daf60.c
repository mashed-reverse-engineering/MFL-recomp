/* rwa_soft_voice_resample_005daf60 (0x005daf60): software mixer: resample one playing voice's wave into its 16-bit block */
#include <string.h>
#include "mfl_tier2.h"

#define SOFT_STATE_STOPPING 1      /* state+0x00 */
#define SOFT_STATE_PLAYING  4
#define SOFT_WAVE_LOOPED    8      /* state+0xa4 flags */
#define RATE_ONE            ((uint64_t)1 << 32)
#define INT32_INDEFINITE    (-2147483647 - 1)      /* what a 32-bit fistp stores for NaN and out-of-range values */
#define CHOP(v) (((v) >= -2147483648.0f && (v) < 2147483648.0f) ? (int)(v) : INT32_INDEFINITE)

/* Linear interpolation between two 16-bit samples, 15-bit fraction. */
#define LERP(a, b, frac) ((short)(((((b) - (a)) * (frac)) >> 15) + (a)))

/* voice+0x20 -> mix state st: +0x00 state, +0x04 / +0x08 loop start / wave end (frames), +0x44 / +0x48 gains, +0x50 wave samples,
   +0x54 output block, +0x58 step and +0x60 position (32.32 frames, 64 bit), +0x70 -> format (+0x39 channels), +0x74 -> bus
   (+0x28 frames per block), +0xa4 flags.  Fills bus frames of the block (fewer when a one-shot wave ends: the voice then goes to
   "stopping"); a looped wave wraps its position back by (end - loop start).  Silent voices only advance; rate 1.0 is a copy;
   otherwise linear interpolation (mono 4 frames per round + the rest, stereo frame by frame).  The frames to the wave end are
   ceil((end - position) / step), computed in float as the original does.  Loads / stores follow the original's order
   (the position lives in st and is written back every round).  Returns the voice, NULL when it is not playing. */
/* TIER2: unvalidated 2026-11-05 authority=diffemu */
char *rwa_soft_voice_resample_005daf60(char *voice)
{
  char *st = *(char **)(voice + 0x20);
  uint64_t *pos = (uint64_t *)(st + 0x60);
  short *data, *out;
  RwUInt32 remaining, chunk, channels, q, hi, hi1, hi2, hi3;
  int at_end, a0, a1, a2, a3, b0, b1, b2, b3, frac;

  if (*(int *)st != SOFT_STATE_PLAYING)
    return 0;
  data = *(short **)(st + 0x50);
  remaining = *(RwUInt32 *)(*(char **)(st + 0x74) + 0x28);
  channels = (*(unsigned char **)(st + 0x70))[0x39];
  out = *(short **)(st + 0x54);
  if (remaining == 0)
    return voice;
  do {
    uint64_t step = *(uint64_t *)(st + 0x58);

    chunk = remaining;
    at_end = 0;
    if (step != 0) {
      uint64_t left = ((uint64_t)*(RwUInt32 *)(st + 0x08) << 32) - *pos;
      uint64_t rest = left % *(uint64_t *)(st + 0x58);
      float ratio = (float)((double)left / (double)*(uint64_t *)(st + 0x58));
      int64_t frames = CHOP(ratio);

      if (rest != 0)
        frames++;
      if ((uint64_t)frames <= remaining) {
        at_end = 1;
        chunk = (RwUInt32)frames;
      }
    }
    if (*(int *)(st + 0x44) == 0 && *(int *)(st + 0x48) == 0) {      /* silent: advance only */
      out += chunk * channels;
      *pos += *(uint64_t *)(st + 0x58) * chunk;
    } else if (*(uint64_t *)(st + 0x58) == RATE_ONE) {      /* rate 1.0: copy */
      RwUInt32 bytes = chunk * channels * 2;

      memcpy(out, data + (RwUInt32)(*pos >> 32) * channels, bytes);
      out = (short *)((char *)out + bytes);
      *pos += (uint64_t)chunk << 32;
    } else if (channels == 1) {      /* mono, linear interpolation */
      uint64_t s1 = *(uint64_t *)(st + 0x58);      /* position steps for 1..4 frames */
      uint64_t s2 = s1 << 1, s4 = s1 << 2, s3 = s1 * 3;

      for (q = chunk >> 2; q != 0; q--) {
        uint64_t p0 = *pos, p1 = p0 + s1, p2 = p0 + s2, p3 = p0 + s3;

        hi = (RwUInt32)(p0 >> 32);
        hi1 = (RwUInt32)(p1 >> 32);
        hi2 = (RwUInt32)(p2 >> 32);
        hi3 = (RwUInt32)(p3 >> 32);
        a0 = data[hi];
        a1 = data[hi1];
        b1 = data[hi1 + 1];
        b0 = data[hi + 1];
        out[0] = LERP(a0, b0, (int)((RwUInt32)p0 >> 17));
        a2 = data[hi2];
        b2 = data[hi2 + 1];
        out[1] = LERP(a1, b1, (int)((RwUInt32)p1 >> 17));
        a3 = data[hi3];
        b3 = data[hi3 + 1];
        out[2] = LERP(a2, b2, (int)((RwUInt32)p2 >> 17));
        *pos += s4;
        out[3] = LERP(a3, b3, (int)((RwUInt32)p3 >> 17));
        out += 4;
      }
      for (q = chunk & 3; q != 0; q--) {
        hi = (RwUInt32)(*pos >> 32);
        a0 = data[hi];
        b0 = data[hi + 1];
        frac = (int)((RwUInt32)(*pos >> 17) & 0x7fff);
        *out++ = LERP(a0, b0, frac);
        *pos += *(uint64_t *)(st + 0x58);
      }
    } else {      /* stereo, linear interpolation */
      for (q = chunk; q != 0; q--) {
        hi = (RwUInt32)(*pos >> 32) * 2;
        frac = (int)((RwUInt32)(*pos >> 17) & 0x7fff);
        a0 = data[hi];
        a1 = data[hi + 1];
        b1 = data[hi + 3];
        b0 = data[hi + 2];
        out[0] = LERP(a0, b0, frac);
        out[1] = LERP(a1, b1, frac);
        out += 2;
        *pos += *(uint64_t *)(st + 0x58);
      }
    }
    if (at_end == 1) {
      if (*(unsigned char *)(st + 0xa4) & SOFT_WAVE_LOOPED) {
        if ((RwUInt32)(*pos >> 32) >= *(RwUInt32 *)(st + 0x08))
          *pos -= (uint64_t)(RwUInt32)(*(int *)(st + 0x08) - *(int *)(st + 0x04)) << 32;
      } else {
        *(int *)st = SOFT_STATE_STOPPING;
        remaining = chunk;
      }
    }
    remaining -= chunk;
  } while (remaining != 0);
  return voice;
}
