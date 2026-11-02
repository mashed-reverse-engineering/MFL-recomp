/* rwp_aggregate_vslot_get_hull_words (0x005847e0): aggregate shape virtual method returning four hull words */
#include "mfl_tier2.h"

typedef struct MflAggData {
  unsigned char pad00[4];
  void *hull;   /* 0x4 */
  unsigned char pad08[0x20];
  int plane[4];   /* 0x28: four words (a plane / sphere record) */
  unsigned char pad38[0x30];
  unsigned int flags;   /* 0x68 */
} MflAggData;
typedef struct MflAggShape {
  unsigned char pad00[0x40];
  MflAggData *data;   /* 0x40 */
} MflAggShape;

/* Virtual method of the aggregate shape: when its data has the bit 8 flag and a hull, copies the four words at 0x28 to `out` and returns `out`,
   otherwise returns 0. */
/* TIER2: unvalidated 2026-10-31 authority=diffemu */
int *rwp_aggregate_vslot_get_hull_words(MflAggShape *s, int *out)
{
  MflAggData *d = s->data;

  if (!(d->flags & 8)) return 0;
  if (!d->hull) return 0;
  out[0] = d->plane[0];
  out[1] = d->plane[1];
  out[2] = d->plane[2];
  out[3] = d->plane[3];
  return out;
}
