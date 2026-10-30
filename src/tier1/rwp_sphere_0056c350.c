/* compile: /G5 */
#include "mfl_tier2.h"
typedef struct RwpShape RwpShape;
typedef struct RwpShapeOps { char pad[0x10]; int (*query)(RwpShape *s, RwMatrix *m, int a, int b); char pad2[0x2c]; unsigned char flags; } RwpShapeOps;
struct RwpShape { char pad[0xc]; unsigned flags; char pad2[0x4c]; RwpShapeOps *ops; };
extern RwMatrix *RwMatrixMultiply(RwMatrix *out, RwMatrix *a, RwMatrix *b);
/* Calls the shape ops query with the matrix combined with the shape's own matrix (unless the shape is flagged to use the matrix as is). (Rwp library) */
int rwp_sphere_0056c350(RwpShape *s, RwMatrix *m, int a, int b)
{
  RwMatrix product;
  if (!(s->ops->flags & 2) && !(s->flags & 0x20000)) {
    if (m) m = RwMatrixMultiply(&product, (RwMatrix *)s, m);
    else m = (RwMatrix *)s;
  }
  return s->ops->query(s, m, a, b);
}
