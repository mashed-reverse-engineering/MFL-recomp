/* compile: /G5 */
/* Box around the two points a and b grown by the margin m on every side (max at +0, min at +0x10). */
typedef struct { float x, y, z; } V3;
typedef struct { V3 max; float pad; V3 min; } RwpBounds;
void rwp_math_0056f500(RwpBounds *out, V3 *a, V3 *b, float m)
{
  if (a->x < b->x) {
    out->min.x = a->x - m;
    out->max.x = m + b->x;
  } else {
    out->min.x = b->x - m;
    out->max.x = m + a->x;
  }
  if (a->y < b->y) {
    out->min.y = a->y - m;
    out->max.y = m + b->y;
  } else {
    out->min.y = b->y - m;
    out->max.y = m + a->y;
  }
  if (a->z < b->z) {
    out->min.z = a->z - m;
    out->max.z = m + b->z;
  } else {
    out->min.z = b->z - m;
    out->max.z = m + a->z;
  }
}
