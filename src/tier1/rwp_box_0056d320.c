/* compile: /G5 */
typedef struct { float x, y, z; } V3;
typedef struct { float pad[16]; V3 half; } RwpBox;
/* Support corner of a box in the direction dir: the half extents with the sign of each dir component. */
V3 *rwp_box_0056d320(RwpBox *box, V3 *dir, V3 *out)
{
  out->x = (dir->x < 0.0f ? -1.0f : 1.0f) * box->half.x;
  out->y = (dir->y < 0.0f ? -1.0f : 1.0f) * box->half.y;
  out->z = (dir->z < 0.0f ? -1.0f : 1.0f) * box->half.z;
  return out;
}
