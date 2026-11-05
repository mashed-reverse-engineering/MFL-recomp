/* rwa_soft_voice_update_gains_005d7e80 (0x005d7e80): software mixer voice: left / right / reverb levels from gain and pan, start the ramps */
#include <string.h>
#include "mfl_tier2.h"

/*0064a0e0*/ extern short g_rwa_sqrt_table[];      /* 256 mantissa square roots (upper 16 bits of the float), RenderWare's table sqrt */

#define RAMP_SAMPLES     64
#define INT32_INDEFINITE (-2147483647 - 1)      /* what a 32-bit fistp stores for NaN and out-of-range values */

typedef struct RwaSoftRamp { int samples; int cur[2]; int step[2]; } RwaSoftRamp;      /* gains in 8.8 fixed point */

/* Table square root of a float variable, in place (the bit trick of RenderWare's rwSqrt): halve the exponent, look the
   top 7 mantissa bits up (+128 entries for an odd exponent).  The original masks the whole mantissa and ORs in bit 23
   before taking bits 16..23; only those bits are used, so the index is built from them directly (the 0x7fffff mask
   would look like a data address to objscan).  (Macros, not static functions: one function per tier 2 file.) */
#define TABLE_SQRT(v) do { RwUInt32 bits_, idx_; short exp_; \
    memcpy(&bits_, &(v), 4); \
    exp_ = (short)((bits_ >> 23) - 127); \
    idx_ = (bits_ >> 16) & 0x7f; \
    if (exp_ & 1) idx_ |= 0x80; \
    exp_ >>= 1; \
    bits_ = ((RwUInt32)g_rwa_sqrt_table[idx_] << 16) | ((RwUInt32)(exp_ + 127) << 23); \
    memcpy(&(v), &bits_, 4); } while (0)

/* The original converts with a 32-bit fistp under chop rounding (v: a float variable). */
#define CHOP(v) (((v) >= -2147483648.0f && (v) < 2147483648.0f) ? (int)(v) : INT32_INDEFINITE)

/* 64-sample ramp from the current gains towards new ones (same as rwa_soft_gain_ramp_start_005d7e30). */
#define RAMP_START(r, left, right) do { \
    (r)->step[0] = (((left) << 8) - (r)->cur[0]) / RAMP_SAMPLES; \
    (r)->step[1] = (((right) << 8) - (r)->cur[1]) / RAMP_SAMPLES; \
    (r)->samples = ((r)->step[0] != 0 || (r)->step[1] != 0) ? RAMP_SAMPLES : 0; } while (0)

/* voice+0x9c -> mixer (+0x08 master gain); voice+0x3c / +0x44 gains, +0x48 pan (-1 .. 1), +0x40 reverb send.  Levels 0..255 go
   to +0x6c (left), +0x70 (right), +0x74 (reverb); the dry ramp is at +0xa4, the reverb ramp (both sides +0x74) at +0xb8. */
/* TIER2: unvalidated 2026-11-05 authority=diffemu */
void __fastcall rwa_soft_voice_update_gains_005d7e80(char *voice)
{
  float gain = *(float *)(*(char **)(voice + 0x9c) + 8) * *(float *)(voice + 0x44) * *(float *)(voice + 0x3c);
  float right = *(float *)(voice + 0x48) * 0.5f + 0.5f;
  float left = 1.0f - right;
  float level;
  RwaSoftRamp *dry = (RwaSoftRamp *)(voice + 0xa4);
  RwaSoftRamp *wet = (RwaSoftRamp *)(voice + 0xb8);

  if (left != 0.0f)
    TABLE_SQRT(left);
  if (right != 0.0f)
    TABLE_SQRT(right);
  level = left * gain * 255.0f;
  *(int *)(voice + 0x6c) = CHOP(level);
  level = right * gain * 255.0f;
  *(int *)(voice + 0x70) = CHOP(level);
  level = *(float *)(voice + 0x40) * 255.0f;
  *(int *)(voice + 0x74) = CHOP(level);
  RAMP_START(dry, *(int *)(voice + 0x6c), *(int *)(voice + 0x70));
  RAMP_START(wet, *(int *)(voice + 0x74), *(int *)(voice + 0x74));
}
