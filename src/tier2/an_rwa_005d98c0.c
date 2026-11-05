/* rwa_soft_dsp_set_channel_gain_005d98c0 (0x005d98c0): software dsp node: set the gain of one channel (kept as overall gain + pan) */
#include "mfl_tier2.h"

#define MIN_GAIN 1e-5f      /* below this the pan is 0 */

typedef struct RwaSoftDspGainParam {
  int output;      /* 0 left, 1 right */
  int input;       /* < 2 */
  float gain;
} RwaSoftDspGainParam;

typedef struct RwaSoftDspState {
  char pad0[0x5c];
  RwUInt32 dirty;        /* +0x5c: 0x40 | 0x80 gain and pan changed */
  char pad60[4];
  float gain;            /* +0x64 the louder channel */
  float pan;             /* +0x68 -1 (left only) .. 1 (right only) */
  char pad6c[0x1c4];
  unsigned char mode;    /* +0x230: 1 mono in (the input picks the channel), 2 stereo (input must equal output) */
} RwaSoftDspState;

/* TIER2: unvalidated 2026-11-05 authority=diffemu */
void rwa_soft_dsp_set_channel_gain_005d98c0(char *node, const RwaSoftDspGainParam *p)
{
  RwaSoftDspState *st = *(RwaSoftDspState **)(node + 0x20);
  unsigned char mode;
  float pan, gain, left, right, peak, inv;

  if (p->input >= 2)
    return;
  mode = st->mode;
  if (mode == 2 && p->input != p->output)
    return;
  pan = st->pan;
  gain = st->gain;
  if (pan >= 0.0f) {
    left = (1.0f - pan) * gain;
    right = gain;
  } else {      /* also NaN */
    left = gain;
    right = (pan + 1.0f) * gain;
  }
  if ((mode == 1 ? p->input : p->output) == 0)
    left = p->gain;
  else
    right = p->gain;
  peak = left > right ? left : right;
  inv = (peak > MIN_GAIN && peak != 0.0f) ? 1.0f / peak : 0.0f;
  st->dirty |= 0xc0;
  st->pan = right * inv - left * inv;
  st->gain = peak;
}
