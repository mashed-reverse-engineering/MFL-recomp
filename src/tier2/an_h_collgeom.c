#include "mfl_tier2.h"
/* RenderWare collision plugin (rpcollis): RpCollisionGeometryForAllIntersections (0x54acd0), the dispatcher on the
   intersection type.  Written from the disassembly of MFL.exe.  GeometryForAllTriIntsecBBox (0x54bdc0) is a trap row:
   the original calls it at 0x54ad3c too (tools/delink/trap_caller_allow.txt line). */

enum {                                    /* RpIntersectType */
  rpINTERSECTLINE = 1,
  rpINTERSECTPOINT = 2,
  rpINTERSECTSPHERE = 3,
  rpINTERSECTBOX = 4,
  rpINTERSECTATOMIC = 5
};

typedef struct RpIntersection {
  union {
    char shape[0x18];                     /* RwLine / RwV3d / RwSphere / RwBBox at the start */
    void *object;                         /* rpINTERSECTATOMIC: the RpAtomic */
  } t;
  RwInt32 type;                           /* 0x18 */
} RpIntersection;

typedef void *(*RpIntersectionCallBackGeometryTriangle)(void *intersection, void *triangle, void *data);

/*0054adb0*/ extern void GeometryForAllTriIntsecLine(void *geometry, const void *line, RpIntersection *intersection,
                                                     RpIntersectionCallBackGeometryTriangle callback, void *data);
/*0054c400*/ extern void GeometryForAllTriIntsecSphere(void *geometry, const RwSphere *sphere, RpIntersection *intersection,
                                                       RpIntersectionCallBackGeometryTriangle callback, void *data);
/*0054bdc0*/ extern void GeometryForAllTriIntsecBBox(void *geometry, RpIntersection *intersection,
                                                     RpIntersectionCallBackGeometryTriangle callback, void *data);
/*004f6240*/ extern const RwSphere *RpAtomicGetWorldBoundingSphere(void *atomic);

void *RpCollisionGeometryForAllIntersections(void *geometry, RpIntersection *intersection,
                                             RpIntersectionCallBackGeometryTriangle callback, void *data)
{
  RwSphere sphere;

  switch (intersection->type) {
  case rpINTERSECTLINE:
    GeometryForAllTriIntsecLine(geometry, intersection->t.shape, intersection, callback, data);
    return geometry;
  case rpINTERSECTSPHERE:
    GeometryForAllTriIntsecSphere(geometry, (const RwSphere *)intersection->t.shape, intersection, callback, data);
    return geometry;
  case rpINTERSECTBOX:
    GeometryForAllTriIntsecBBox(geometry, intersection, callback, data);
    return geometry;
  case rpINTERSECTATOMIC:
    sphere = *RpAtomicGetWorldBoundingSphere(intersection->t.object);
    GeometryForAllTriIntsecSphere(geometry, &sphere, intersection, callback, data);
    return geometry;
  }
  return geometry;      /* rpINTERSECTPOINT and unknown types: nothing to test */
}
