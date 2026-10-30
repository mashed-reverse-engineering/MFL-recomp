/* compile: /G5 */
/* Axis aligned plane test: dist = n[axis] * s - c.  Returns 1 when dist < 0 (writes hit = (0, 1) if hit is given); otherwise writes (dist, -(p[axis] * s)) and returns -1, 0 or 1 by the sign of the second value. */
typedef struct { float dist, other; } RwpPlaneHit;
int rwp_grid_005772c0(RwpPlaneHit *hit, float *n, float *p, int axis, float s, float c)
{
  float dist;
  int r;
  float t;

  t = n[axis];
  dist = t * s - c;
  r = dist < 0.0f;
  if (hit) {
    float other;

    if (!r) {
      hit->dist = dist;
      t = p[axis];
      other = -(t * s);
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
