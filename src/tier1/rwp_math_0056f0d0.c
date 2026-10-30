/* compile: /G5 */
/* Grows the bounding sphere s to include the point p and stores the result in out: if p lies outside, the new sphere touches the far side of s and p, otherwise s is copied. */
typedef struct { float x, y, z; } V3;
typedef struct { V3 c; float r; } RwpSphere;
/*004d53a0*/ extern float RwV3dLength(V3 *v);
RwpSphere *rwp_math_0056f0d0(RwpSphere *out, RwpSphere *s, V3 *p)
{
  V3 d;
  float len;

  d.x = p->x - s->c.x;
  d.y = p->y - s->c.y;
  d.z = p->z - s->c.z;
  len = RwV3dLength(&d);
  if (len > s->r) {
    out->c.x = (len - s->r) / (len + len) * d.x + s->c.x;
    out->c.y = (len - s->r) / (len + len) * d.y + s->c.y;
    out->c.z = (len - s->r) / (len + len) * d.z + s->c.z;
    out->r = (len + s->r) * 0.5f;
    return out;
  }
  if (out != s) {
    out->c.x = s->c.x;
    out->c.y = s->c.y;
    out->c.z = s->c.z;
    out->r = s->r;
  }
  return out;
}
