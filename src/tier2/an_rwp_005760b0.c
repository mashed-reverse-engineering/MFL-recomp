/* rwp_farfield_report_overlaps (0x005760b0): report the pairs of an object box with every farfield object whose box overlaps it */
#include "mfl_tier2.h"
#include "rwp_common.h"
#include "rwp_farfield.h"

/*007daa08*/ extern int g_data_7daa08;
/*007daa0c*/ extern int g_data_7daa0c;
extern void rwp_capsule_00575860(void *pool, unsigned short va, unsigned short vb);

/* Walks the cell tree of the farfield index and reports the pair (id, object) to the edge pool (575860) for every object whose box overlaps
   the query box q, unless the pair filter at +0xc018 (bit group(id) * stride + group(object), groups are u16 at +0xb810) rejects it.  The
   walk keeps a stack of (cell, box): the objects on the cell's own list are tested, then the octants of the cell the query reaches (the
   children overlap: the lower child covers the first 60% of an axis, the upper one the last 60%): a leaf octant's list is tested, a child
   cell is pushed with its sub box.  g_data_7daa08 counts the box tests, g_data_7daa0c the reported pairs. */
/* TIER2: unvalidated 2026-11-02 authority=diffemu */
void rwp_farfield_report_overlaps(unsigned char *f, MflFfBox *q, unsigned int id)
{
  unsigned short stk_cell[320];
  MflFfBox stk_box[320];
  MflFfBox cur;
  MflPairFilter *flt;
  MflFfBox *eb;
  float up_min[3], lo_max[3];      /* the upper child starts at up_min, the lower one ends at lo_max */
  float t0, t1, t2;
  unsigned int cell, r, o, m, k, bi, ix;
  unsigned short w;
  int n, bx, by, bz;

#define FF_TEST_LIST(first)                                                                                         \
  do {                                                                                                              \
    r = (first);                                                                                                    \
    do {                                                                                                            \
      g_data_7daa08++;                                                                                              \
      o = FF_OWNER(f, r) & 0x3ff;                                                                                   \
      eb = FF_E(f, o);                                                                                              \
      if (eb->min[0] <= q->max[0] && q->min[0] <= eb->max[0] && eb->min[1] <= q->max[1] && q->min[1] <= eb->max[1]  \
          && eb->min[2] <= q->max[2] && q->min[2] <= eb->max[2]) {                                                  \
        flt = *(MflPairFilter **)(f + 0xc018);                                                                      \
        if (flt) {                                                                                                  \
          ix = *(unsigned short *)(f + 0xb810 + o * 2) + *(unsigned short *)(f + 0xb810 + (id & 0xffff) * 2) * flt->stride; \
          bi = 1u << (ix & 31);                                                                                     \
        }                                                                                                           \
        if (!flt || !(flt->bits[ix >> 5] & bi)) {                                                                   \
          g_data_7daa0c++;                                                                                          \
          rwp_capsule_00575860(*(void **)(f + 0xc010), id & 0xffff, o);                                             \
        }                                                                                                           \
      }                                                                                                             \
      r = FF_NEXT(f, r);                                                                                            \
    } while (r != 0x3ff);                                                                                           \
  } while (0)

  stk_box[0] = *FF_E(f, 0x3ff);
  stk_cell[0] = 0;
  n = 1;
  do {
    n--;
    cell = stk_cell[n];
    cur = stk_box[n];
    if ((FF_HEAD(f, cell) & 0x3ff) != 0x3ff) FF_TEST_LIST(FF_HEAD(f, cell) & 0x3ff);
    t0 = cur.min[0] * 0.6f;
    t1 = cur.min[1] * 0.6f;
    t2 = cur.min[2] * 0.6f;
    up_min[0] = cur.max[0] * 0.39999998f + t0;
    up_min[1] = cur.max[1] * 0.39999998f + t1;
    up_min[2] = cur.max[2] * 0.39999998f + t2;
    t0 = cur.max[0] * 0.6f;
    t1 = cur.max[1] * 0.6f;
    t2 = cur.max[2] * 0.6f;
    lo_max[0] = cur.min[0] * 0.39999998f + t0;
    lo_max[1] = cur.min[1] * 0.39999998f + t1;
    lo_max[2] = cur.min[2] * 0.39999998f + t2;
    /* octants (bit 0 x, bit 1 y, bit 2 z) the query reaches */
    m = 0xff;
    if (q->min[0] > lo_max[0]) m = 0xaa;
    if (q->max[0] < up_min[0]) m &= 0x55;
    if (q->min[1] > lo_max[1]) m &= 0xcc;
    if (q->max[1] < up_min[1]) m &= 0x33;
    if (q->min[2] > lo_max[2]) m &= 0xf0;
    if (q->max[2] < up_min[2]) m &= 0x0f;
    for (k = 0; k < 8; k++) {
      if (!(m & (1u << k))) continue;
      w = FF_SLOT(f, cell, k);
      if (w & 0x8000) {
        if ((w & 0x3ff) != 0x3ff) FF_TEST_LIST(w & 0x3ff);
      } else {
        stk_cell[n] = w;
        bx = k & 1;
        by = (k >> 1) & 1;
        bz = (k >> 2) & 1;
        stk_box[n].min[0] = (float)(1 - bx) * cur.min[0] + (float)bx * up_min[0];
        stk_box[n].min[1] = (float)(1 - by) * cur.min[1] + (float)by * up_min[1];
        stk_box[n].min[2] = (float)(1 - bz) * cur.min[2] + (float)bz * up_min[2];
        stk_box[n].max[0] = (float)bx * cur.max[0] + (float)(1 - bx) * lo_max[0];
        stk_box[n].max[1] = (float)by * cur.max[1] + (float)(1 - by) * lo_max[1];
        stk_box[n].max[2] = (float)bz * cur.max[2] + (float)(1 - bz) * lo_max[2];
        n++;
      }
    }
  } while (n > 0);
#undef FF_TEST_LIST
}
