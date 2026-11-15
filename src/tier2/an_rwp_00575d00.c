/* rwp_farfield_boxes_shrink (0x00575d00): shrink two boxes towards each other and classify the cut positions */
#include "mfl_tier2.h"

extern int _rand(void);

/* A box: min corner at 0 and max corner at 0x10 (padded to 4 floats each). */
typedef struct MflClipBox {
  float min[3];
  float pad0c;
  float max[3];
  float pad1c;
} MflClipBox;

/* Shrinks the box `b` (second argument) towards the box `a` (third): both boxes are cut to 40% / 60% blends of their corners,
   the cut positions are classified against the other box (bit k set when the blended max of a is above b.min[k] ... as outcodes), and
   when the two outcodes together cover all three axes one of the axes (the ones the codes share are picked by _rand()) selects
   which of the blended or original corner is kept per axis.  Writes the new box to `out` and returns the axis selection mask (0..7),
   or 0xffff when the outcodes do not cover all three axes. */
/* TIER2: unvalidated 2026-10-31 authority=diffemu */
int rwp_farfield_boxes_shrink(MflClipBox *out, MflClipBox *b, MflClipBox *a)
{
  int r = _rand();
  float hx, hz, lx, lz;
  float lyf;
  volatile float hz_term, lz_term;      /* the third corner terms are rounded to frame floats like the original (fstp, reloaded by fadd) */
  double hy, ly;
  int code_hi, code_lo, common, sel;
  int bit_x, bit_y, bit_z;

  hy = a->max[1] * 0.6f;
  hz_term = a->max[2] * 0.6f;
  hx = a->min[0] * 0.39999998f + a->max[0] * 0.6f;
  hy = a->min[1] * 0.39999998f + hy;
  hz = a->min[2] * 0.39999998f + hz_term;
  ly = a->min[1] * 0.6f;
  lz_term = a->min[2] * 0.6f;
  lx = a->max[0] * 0.39999998f + a->min[0] * 0.6f;
  ly = a->max[1] * 0.39999998f + ly;
  lyf = ly;
  lz = a->max[2] * 0.39999998f + lz_term;
  code_hi = (b->max[0] > hx) + 2 * ((b->max[1] > hy) + 2 * (b->max[2] > hz));
  code_lo = (lx > b->min[0]) + 2 * ((ly > b->min[1]) + 2 * (lz > b->min[2]));
  if (((code_hi | code_lo) & 0xffff) != 7) return 0xffff;
  common = code_hi & code_lo;
  sel = (code_hi & ~common) | (r & common);
  bit_x = sel & 1;
  bit_y = (sel >> 1) & 1;
  bit_z = (sel >> 2) & 1;
  out->max[0] = (1 - bit_x) * a->max[0] + bit_x * hx;
  out->max[1] = (1 - bit_y) * a->max[1] + bit_y * hy;
  out->max[2] = (1 - bit_z) * a->max[2] + bit_z * hz;
  out->min[0] = bit_x * a->min[0] + (1 - bit_x) * lx;
  out->min[1] = bit_y * a->min[1] + (1 - bit_y) * lyf;
  out->min[2] = bit_z * a->min[2] + (1 - bit_z) * lz;
  return sel;
}
