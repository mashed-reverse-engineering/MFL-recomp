#include "mfl_tier2.h"

extern int rwp_pipeline_00570f20(void *w);
extern int rwp_batch_00578e50(void *w);
extern int rwp_batch_00578e80(void *w);
extern int rwp_batch_005793a0(int n);
extern int rwp_batch_005792f0(void *w);
extern int rwp_grid_00577ce0(void *w);
extern int rwp_grid_00577970(void *w);

/* The sizes of a world description (counts in n..) and the byte offsets of its parts (o..) filled in by rwp_pipeline_00571070. */
typedef struct MflWorldLayout {
  unsigned int flags;   /* 0x0 feature flags (a bit set */
  unsigned char pad04[0x4];
  int n8;   /* 0x8 */
  int nc;   /* 0xc */
  int n10;   /* 0x10 */
  unsigned char pad14[0x8];
  int n1c;   /* 0x1c */
  int n20;   /* 0x20 */
  unsigned char pad24[0x18];
  int n3c;   /* 0x3c */
  int o40;   /* 0x40 */
  int o44;   /* 0x44 */
  int o48;   /* 0x48 */
  int o4c;   /* 0x4c */
  int o50;   /* 0x50 */
  int o54;   /* 0x54 */
  int o58;   /* 0x58 */
  int o5c;   /* 0x5c */
  int o60;   /* 0x60 */
  int o64;   /* 0x64 */
  int o68;   /* 0x68 */
  int o6c;   /* 0x6c */
  int o70;   /* 0x70 */
  int o74;   /* 0x74 */
  int o78;   /* 0x78 */
} MflWorldLayout;


#define ALIGN16(x) (((x) + 15) & ~15)

/* Lays out the memory block of a physics world: every part not excluded by a feature flag gets a 16 byte aligned size, stored as
   offset / size field in the description, and the sum of all sizes is returned. */
/* TIER2: unvalidated 2026-10-31 authority=diffemu */
int rwp_pipeline_00571070(MflWorldLayout *w)
{
  int total, a, b, c, d, e, n;
  unsigned int f;

  total = rwp_pipeline_00570f20(w);
  a = ALIGN16(w->n10 * 4);
  b = ALIGN16(w->n1c * 5 * 4);
  w->o40 = a;
  w->o6c = b;
  total = total + (b + a);
  if (!(w->flags & 2)) {
    n = rwp_batch_00578e50(w);
    w->o64 = n;
    total = total + n;
  }
  c = ALIGN16(w->nc * 4);
  d = ALIGN16(w->n10 * 5 * 8);
  e = ALIGN16(w->n10 * 4);
  w->o68 = (c + d) + e;
  n = w->n20;
  w->o4c = ALIGN16(n * 7 * 32);
  total = total + ((c + d) + e + w->o4c);
  if (!(w->flags & 0x20)) {
    w->o54 = ALIGN16(n * 16) + ALIGN16(n * 4);
    total = total + w->o54;
  }
  w->o70 = c;
  total = total + c;
  w->o74 = rwp_batch_005793a0(n);
  total = total + w->o74;
  if (!(w->flags & 8)) {
    n = rwp_batch_00578e80(w);
    w->o50 = n;
    total = total + n;
  }
  a = ALIGN16(w->n20 * 5 * 4);
  f = w->flags;
  total = total + a;
  w->o78 = a;
  if (!(f & 4)) {
    n = w->n3c + 1;
    d = ALIGN16(w->n8 * 2);
    e = ALIGN16(n * 4);
    e = d + e * 2;
    e = e + ALIGN16(n * 32);
    e = e + ALIGN16(n * 16);
    a = ALIGN16(n * 3 * 4);
    a = a + e;
    w->o5c = a;
    total = total + a;
  }
  if (!(f & 1)) {
    n = rwp_batch_005792f0(w);
    w->o60 = n;
    total = total + n;
  }
  if (!(w->flags & 0x10)) {
    w->o44 = rwp_grid_00577ce0(w);
    w->o48 = rwp_grid_00577970(w);
    total = total + (w->o44 + w->o48);
  }
  if (!(w->flags & 0x40)) {
    a = ALIGN16(w->n20 * 64);
    w->o58 = a;
    total = total + a;
  }
  return total;
}
