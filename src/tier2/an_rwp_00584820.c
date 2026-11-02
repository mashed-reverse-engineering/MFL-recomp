/* rwp_aggregate_bounding_data (0x00584820): bounding data virtual method of the aggregate shape */
#include "mfl_tier2.h"
#include "rwp_common.h"

typedef struct MflAggData {
  int src_a;   /* 0x0 */
  int src_b;   /* 0x4 */
  unsigned char pad08[0x30];
  float radius;   /* 0x38: negative = not computed yet */
  MflAxis3 axis;   /* 0x3c */
  MflExt7 ext;   /* 0x48 */
} MflAggData;
typedef struct MflAggShape {
  unsigned char pad00[0x40];
  MflAggData *data;   /* 0x40 */
} MflAggShape;

extern float rwp_aggregate_00584960(int src_a, int src_b, MflExt7 *ext, MflAxis3 *axis, int mode);

/* Virtual method of the aggregate shape: bounding data of the shape.  The radius is computed on first use (584960); the extent record (7 words)
   and the axis (3 words) are copied out, the radius is returned. */
/* TIER2: unvalidated 2026-10-31 authority=diffemu */
float rwp_aggregate_bounding_data(MflAggShape *s, MflExt7 *ext, MflAxis3 *axis)
{
  MflAggData *d = s->data;

  if (d->radius < 0.0f) d->radius = rwp_aggregate_00584960(d->src_a, d->src_b, &d->ext, &d->axis, 0);
  *ext = d->ext;
  axis->w[0] = d->axis.w[0];
  axis->w[1] = d->axis.w[1];
  axis->w[2] = d->axis.w[2];
  return d->radius;
}
