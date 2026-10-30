/* compile: /G5 */
/* Plane test of a direction d against the plane (n, c): dist = n . d - c.  Returns 1 when dist <= 0 (writes hit = (0, 1) if hit is given); otherwise writes (dist, -(p . d)) and returns -1, 0 or 1 by the sign of the second value. */
typedef struct { float dist, other; } RwpPlaneHit;
int rwp_grid_00577350(RwpPlaneHit *hit, float *n, float *p, float *d, float c)
{
  float dist;
  int r;

  dist = (n[1] * d[1] + n[0] * d[0]) + n[2] * d[2] - c;
  r = dist <= 0.0f;
  if (hit) {
    float other;

    if (!r) {
      hit->dist = dist;
      other = -((p[1] * d[1] + p[0] * d[0]) + p[2] * d[2]);
      hit->other = other;
      if (dist > other) {
        return other < 0.0f ? -1 : 0;
      }
    } else {
      hit->dist = 0;
      hit->other = 1.0f;
    }
    return 1;
  }
  return r;
}
