#include "mfl_tier2.h"

extern int rwp_sphere_0056c0b0(void *obj, void *mat, void *out);

typedef struct MflSphereTypeData {
  unsigned char pad00[0x40];
  unsigned int flags;   /* 0x40 */
} MflSphereTypeData;
/* A sphere object: starts with its 4x4 matrix, flags word at 0xc, type data at 0x5c. */
typedef struct MflSphereObj {
  float mat[3];
  unsigned int flags;   /* 0xc */
  unsigned char pad10[0x4c];
  MflSphereTypeData *data;   /* 0x5c */
} MflSphereObj;

/* Calls 56c0b0 with the object and the matrix to use: the object's own when its type data has bit 2 or the object bit 0x20000, else the
   matrix `m` concatenated with the object's matrix (m == 0: the object's matrix alone). */
/* TIER2: unvalidated 2026-10-31 authority=diffemu */
int rwp_sphere_0056c140(MflSphereObj *obj, void *m, void *out)
{
  float tmp[16];

  if (!(obj->data->flags & 2)) {
    if (!(obj->flags & 0x20000)) {
      if (m) {
        return rwp_sphere_0056c0b0(obj, RwMatrixMultiply((RwMatrix *)tmp, (RwMatrix *)obj, (RwMatrix *)m), out);
      }
      return rwp_sphere_0056c0b0(obj, obj, out);
    }
  }
  return rwp_sphere_0056c0b0(obj, m, out);
}
