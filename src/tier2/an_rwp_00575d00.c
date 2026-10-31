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
int rwp_farfield_00575d00(MflClipBox *out, MflClipBox *b, MflClipBox *a)
{
  int r = _rand();
  float hx, hz, lx, lz;
  float lyf;
  volatile float t18, t2;      /* the third corner terms are rounded to frame floats like the original (fstp, reloaded by fadd) */
  double hy, ly;
  int c1, c2, common, sel;
  int b0, b1, b2;

  hy = a->max[1] * 0.6f;
  t18 = a->max[2] * 0.6f;
  hx = a->min[0] * 0.39999998f + a->max[0] * 0.6f;
  hy = a->min[1] * 0.39999998f + hy;
  hz = a->min[2] * 0.39999998f + t18;
  ly = a->min[1] * 0.6f;
  t2 = a->min[2] * 0.6f;
  lx = a->max[0] * 0.39999998f + a->min[0] * 0.6f;
  ly = a->max[1] * 0.39999998f + ly;
  lyf = ly;
  lz = a->max[2] * 0.39999998f + t2;
  c1 = (b->max[0] > hx) + 2 * ((b->max[1] > hy) + 2 * (b->max[2] > hz));
  c2 = (lx > b->min[0]) + 2 * ((ly > b->min[1]) + 2 * (lz > b->min[2]));
  if (((c1 | c2) & 0xffff) != 7) return 0xffff;
  common = c1 & c2;
  sel = (c1 & ~common) | (r & common);
  b0 = sel & 1;
  b1 = (sel >> 1) & 1;
  b2 = (sel >> 2) & 1;
  out->max[0] = (1 - b0) * a->max[0] + b0 * hx;
  out->max[1] = (1 - b1) * a->max[1] + b1 * hy;
  out->max[2] = (1 - b2) * a->max[2] + b2 * hz;
  out->min[0] = b0 * a->min[0] + (1 - b0) * lx;
  out->min[1] = b1 * a->min[1] + (1 - b1) * lyf;
  out->min[2] = b2 * a->min[2] + (1 - b2) * lz;
  return sel;
}
