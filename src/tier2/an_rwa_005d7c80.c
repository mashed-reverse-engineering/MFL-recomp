/* rwa_soft_get_3d_record_005d7c80 (0x005d7c80): software mixer: fill a 3d record (vector, 1, -FLT_MAX, float) from an object */
#include "mfl_tier2.h"

typedef struct RwaSoftVec3 { float x, y, z; } RwaSoftVec3;
typedef struct RwaSoft3dRecord { RwaSoftVec3 v; float min_dist; float value; } RwaSoft3dRecord;      /* 0x14 bytes */

/* src NULL: the defaults {0, 0, 1}, -FLT_MAX, 0.  Otherwise the vector at src+0x20 and the float at src+0x38.  Returns 1. */
/* TIER2: unvalidated 2026-11-05 authority=diffemu */
int rwa_soft_get_3d_record_005d7c80(void *unused, char *src, RwaSoft3dRecord *out)
{
  (void)unused;
  if (!src) {
    out->v.x = 0.0f;
    out->v.y = 0.0f;
    out->value = 0.0f;
    out->v.z = 1.0f;
    out->min_dist = -3.402823466e+38f;
    return 1;
  }
  out->v = *(const RwaSoftVec3 *)(src + 0x20);      /* copied as raw words */
  out->min_dist = -3.402823466e+38f;
  out->value = *(float *)(src + 0x38);
  return 1;
}
