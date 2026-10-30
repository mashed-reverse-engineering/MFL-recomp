/* compile: /G5 */
#include "mfl_tier2.h"
typedef struct { float m[8]; } RwpBox32;
typedef struct RwpPrimitive { char pad[8]; RwpBox32 box; char pad2[0x40]; unsigned flags; } RwpPrimitive;
typedef struct RwpAggregate { char pad[0x40]; RwpPrimitive *primitive; } RwpAggregate;
extern void rwp_math_0056f5b0(RwpBox32 *dst, RwpBox32 *src, void *xform);
/* Copies the primitive's 32 byte box (when it has one) and optionally transforms it in place; returns the destination or NULL. (Rwp library) */
RwpBox32 *rwp_aggregate_00584890(RwpAggregate *a, void *xform, int unused, RwpBox32 *out)
{
  RwpPrimitive *p = a->primitive;
  if (!(p->flags & 4)) return 0;
  *out = p->box;
  if (xform) rwp_math_0056f5b0(out, out, xform);
  return out;
}
