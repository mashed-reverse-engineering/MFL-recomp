/* compile: /G5 */
#include "mfl_tier2.h"
typedef struct { float x, y, z, w; } RwpQuat;
/* Copies a 16-byte quaternion; returns the destination. */
RwpQuat *course_neustein_cb0_sub_56e9b0(RwpQuat *dst, RwpQuat *src)
{
  *dst = *src;
  return dst;
}
