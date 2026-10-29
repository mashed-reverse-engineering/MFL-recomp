/* compile: /G5 */
#include "mfl_tier2.h"
typedef struct Vec3 { float x, y, z; } Vec3;
/* x */
Vec3 *RwBBoxInitialize(Vec3 *bb, Vec3 *v)
{
  bb[1] = *v;
  bb[0] = *v;
  return bb;
}
