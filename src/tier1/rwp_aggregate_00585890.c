/* compile: /G5 */
/* Triangle quality measure of the points p, q, r: squared length of the cross product (q - p) x (r - p) divided by (squared length of r - p)^2 + 1. (Rwp library) */
float rwp_aggregate_00585890(float *p, float *q, float *r)
{
  float v[3];
  float c[3];
  double dx, dy, dz; float l2;
  dx = q[0] - p[0];
  dy = q[1] - p[1];
  dz = q[2] - p[2];
  v[0] = r[0] - p[0];
  v[1] = r[1] - p[1];
  v[2] = r[2] - p[2];
  c[0] = v[2] * dy - v[1] * dz;
  c[1] = v[0] * dz - v[2] * dx;
  c[2] = v[1] * dx - v[0] * dy;
  l2 = (v[1] * v[1] + v[0] * v[0]) + v[2] * v[2];
  return ((c[1] * c[1] + c[0] * c[0]) + c[2] * c[2]) / (l2 * l2 + 1.0f);
}
