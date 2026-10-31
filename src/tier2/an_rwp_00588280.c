#include "mfl_tier2.h"

/* Parameter t of the point of the line `origin + t * dir` closest to `point`, clamped to [t_min, t_max] (the
   bounds are scaled by |dir|^2 so the division is only done for the unclamped case); out = origin + t * dir. */
/* TIER2: unvalidated 2026-10-31 authority=diffemu */
void rwp_aggregate_00588280(RwV3d *origin, RwV3d *dir, float t_min, float t_max, RwV3d *point, RwV3d *out)
{
  float dd, dp, t;
  double dy, dz;

  dd = RwV3dDotProductMacro(dir, dir);
  dy = point->y - origin->y;
  dz = point->z - origin->z;
  dp = dir->y * dy + (point->x - origin->x) * dir->x + dir->z * dz;
  t = dd * t_min >= dp ? t_min : dd * t_max <= dp ? t_max : dp / dd;
  out->x = t * dir->x + origin->x;
  out->y = t * dir->y + origin->y;
  out->z = t * dir->z + origin->z;
}
