/* rwp_shape_bounding_data_b (0x00577090): bounding data virtual method of the second aggregate shape type */
#include "mfl_tier2.h"
#include "rwp_common.h"

typedef struct MflAggData {
  unsigned char pad00[0xc];
  int src_a;   /* 0xc */
  int src_b;   /* 0x10 */
  unsigned char pad14[0x6c];
  float radius;   /* 0x80: negative = not computed yet */
  MflAxis3 axis;   /* 0x84 */
  MflExt7 ext;   /* 0x90 */
} MflAggData;
typedef struct MflAggShape {
  unsigned char pad00[0x40];
  MflAggData *data;   /* 0x40 */
} MflAggShape;

extern float rwp_aggregate_00584960(int src_a, int src_b, MflExt7 *ext, MflAxis3 *axis, int mode);

/* Virtual method of the second aggregate shape type (same as 584820, other offsets): bounding data of the shape.  The radius is computed on first use (584960); the extent record (7 words)
   and the axis (3 words) are copied out, the radius is returned. */
/* TIER2: unvalidated 2026-10-31 authority=diffemu */
float rwp_shape_bounding_data_b(MflAggShape *s, MflExt7 *ext, MflAxis3 *axis)
{
  MflAggData *d = s->data;

  if (d->radius < 0.0f) d->radius = rwp_aggregate_00584960(d->src_a, d->src_b, &d->ext, &d->axis, 0);
  *ext = d->ext;
  axis->w[0] = d->axis.w[0];
  axis->w[1] = d->axis.w[1];
  axis->w[2] = d->axis.w[2];
  return d->radius;
}
