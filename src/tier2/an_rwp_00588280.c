/* rwp_line_closest_param (0x00588280): parameter of the point of a line closest to a point, clamped */
#include "mfl_tier2.h"

/* Parameter t of the point of the line `origin + t * dir` closest to `point`, clamped to [t_min, t_max] (the
   bounds are scaled by |dir|^2 so the division is only done for the unclamped case); out = origin + t * dir.
   Returns 1 when t lies strictly inside (t_min, t_max), 0 when it was clamped to a bound (callers store it as the contact kind). */
/* TIER2: unvalidated 2026-10-31 authority=diffemu */
int rwp_line_closest_param(RwV3d *origin, RwV3d *dir, float t_min, float t_max, RwV3d *point, RwV3d *out)
{
  float dd, dp, t;
  double dy, dz;
  int inside = 0;

  dd = RwV3dDotProductMacro(dir, dir);
  dy = point->y - origin->y;
  dz = point->z - origin->z;
  dp = dir->y * dy + (point->x - origin->x) * dir->x + dir->z * dz;
  if (dd * t_min >= dp) {
    t = t_min;
  } else if (dd * t_max <= dp) {
    t = t_max;
  } else {
    t = dp / dd;
    inside = 1;
  }
  out->x = t * dir->x + origin->x;
  out->y = t * dir->y + origin->y;
  out->z = t * dir->z + origin->z;
  return inside;
}
