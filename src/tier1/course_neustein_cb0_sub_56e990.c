/* compile: /G5 */
#include "mfl_tier2.h"
typedef struct { float x, y, z; } RwpV3;
typedef struct RwpObj { char pad[0x10]; RwpV3 v; } RwpObj;
/* Copies a 12-byte vector into the object at +0x10; returns the object. */
RwpObj *course_neustein_cb0_sub_56e990(RwpObj *o, RwpV3 *v)
{
  o->v = *v;
  return o;
}
